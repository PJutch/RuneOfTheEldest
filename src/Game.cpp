/* This file is part of the Rune of the Eldest.
The Rune of the Eldest - Roguelike about the mage seeking for ancient knowledges
Copyright (C) 2023  PJutch

The Rune of the Eldest is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

The Rune of the Eldest is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with the Rune of the Eldest.
If not, see <https://www.gnu.org/licenses/>. */

#include "Game.hpp"

#include "render/draw/World.hpp"
#include "render/draw/DeathScreen.hpp"
#include "render/draw/Hud.hpp"
#include "render/draw/LevelUpScreen.hpp"
#include "render/ParticleManager.hpp"
#include "render/PlayerMap.hpp"
#include "render/Camera/Camera.hpp"

#include "generation/DungeonGenerator.hpp"

#include "core/World.hpp"
#include "core/ActorSpawner.hpp"
#include "core/XpManager.hpp"
#include "core/Controller/PlayerController.hpp"
#include "core/Controller/EnemyAi.hpp"

#include "util/Keyboard.hpp"
#include "util/raycast.hpp"
#include "util/filesystem.hpp"
#include "util/parse.hpp"
#include "util/parseKeyValue.hpp"
#include "util/stringify.hpp"

Game::Game(std::shared_ptr<core::World> newWorld,
           std::unique_ptr<core::ActorSpawner> actorSpawner_,
           std::shared_ptr<core::ItemManager> items_,
           std::shared_ptr<core::XpManager> xpManager_,
           std::unique_ptr<generation::DungeonGenerator> newDungeonGenerator,
           render::Context renderContext_,
           std::shared_ptr<util::Raycaster> raycaster,
           util::LoggerFactory& loggerFactory) :
        world{std::move(newWorld)},
        actorSpawner{std::move(actorSpawner_)}, items{std::move(items_)},
        xpManager{std::move(xpManager_)},
        dungeonGenerator_{std::move(newDungeonGenerator)},
        renderContext{std::move(renderContext_)},
        generationLogger{ loggerFactory.create("generation") },
        saveLogger{loggerFactory.create("save")} {
    items->load();

    addOnGenerateListener([camera = renderContext.camera]() { camera->reset(); });
    addOnGenerateListener([playerMap = renderContext.playerMap]() { playerMap->onGenerate(); });
    addOnGenerateListener([particles = renderContext.particles]() { particles->clear(); });
    addOnGenerateListener([raycaster = std::move(raycaster)]() { raycaster->clear(); });
    addOnGenerateListener([xpManager = xpManager]() { xpManager->onGenerate(); });
    addOnGenerateListener([]() { std::filesystem::remove("latest.sav"); });

    addOnUpdateListener([camera = renderContext.camera](sf::Time elapsedTime) { camera->update(elapsedTime); });
    addOnUpdateListener([playerMap = renderContext.playerMap](sf::Time) { playerMap->update(); });
    addOnUpdateListener([particles = renderContext.particles](sf::Time elapsedTime) { particles->update(elapsedTime); });
}

void Game::run() {
    if (auto v = util::readWhole("latest.sav")) {
        loadFromString(*v);
    } else {
        generate();
    }

    sf::Clock clock;
	while (renderContext.window->isOpen()) {
        sf::Event event;
        while (renderContext.window->pollEvent(event))
            handleEvent(event);

        if (world->player().isAlive() && !xpManager->canLevelUp()) {
            world->update();
        }

        sf::Time elapsedTime = clock.restart();
        onUpdate(elapsedTime);

        draw_();
    }

    save();
}

namespace {
    class UnknownSection : public util::RuntimeError {
    public:
        UnknownSection(std::string_view name, util::Stacktrace stacktrace = {}) :
            util::RuntimeError{std::format("Unknown section \"{}\"", name), std::move(stacktrace)} {}
    };

    class ItemNotFound : public util::RuntimeError {
    public:
        ItemNotFound(std::string_view name, util::Stacktrace currentStacktrace = {}) noexcept :
            util::RuntimeError{std::format("Item \"{}\" not found", name), std::move(currentStacktrace)} {}
    };

    struct ItemData {
        std::string id;
        std::string data;
        core::Position<int> position;
    };

    BOOST_DESCRIBE_STRUCT(ItemData, (), (id, data, position))
}

void Game::loadFromString(std::string_view s) {
    saveLogger->info("Loading started...");

    auto multimap = *JutchsON::parse<std::unordered_multimap<std::string, std::string>>(s);
    for (const auto& [key, data] : multimap) {
        if (key == "Tiles") {
            saveLogger->info("Loading tiles...");
            world->tiles() = util::parseCharMap(data).transform(&core::tileFromChar);
        } else if (key == "Stairs") {
            auto [stairs1, stairs2] = *JutchsON::parse<std::pair<sf::Vector3i, sf::Vector3i>>(data);
            world->addStairs(stairs1, stairs2);
        } else if (key == "Actor") {
            auto actor = actorSpawner->parseActor(data);
            world->addActor(std::move(actor));
        } else if (key == "KnownTiles") {
            saveLogger->info("Loading known tiles...");
            renderContext.playerMap->parseTileStates(data);
        } else if (key == "KnownActor") {
            renderContext.playerMap->parseSeenActor(data);
        } else if (key == "Sound") {
            renderContext.playerMap->addSound(core::Sound::parse(data));
        } else if (key == "Xp") {
            saveLogger->info("Loading player leveling data...");
            xpManager->parse(data);
        } else if (key == "Item") {
            auto itemData = JutchsON::parse<ItemData>(data);

            if (auto item = items->newItem(itemData->id)) {
                item->parseData(itemData->data);
                world->addItem(itemData->position, std::move(item));
            } else {
                throw ItemNotFound{itemData->id};
            }
        } else if (key == "KnownItem") {
            renderContext.playerMap->parseSeenItem(data);
        } else if (key == "IdentifiedItems") {
            items->parseIdentifiedItems(data);
        } else if (key == "ItemTextures") {
            items->parseTextures(data);
        } else {
            throw UnknownSection{key};
        }
    }

    for (const auto& [position, item] : world->items()) {
        item->onLoad();
    }

    for (const auto& actor : world->actors()) {
        for (const auto& item : actor->items()) {
            item->onLoad();
        }
        
        for (const auto& slotEquipment : actor->equipment()) {
            for (const auto& piece : slotEquipment) {
                if (piece) {
                    piece->onLoad();
                }
            }
        }
    }

    actorSpawner->onSaveLoaded();
    saveLogger->info("Loading finished");
}

void Game::handleEvent(sf::Event event) {
    if (event.type == sf::Event::Closed
     || util::wasKeyPressed(event, sf::Keyboard::Escape)) {
        renderContext.window->close();
        return;
    }

    if (!world->player().isAlive())
        if (event.type == sf::Event::KeyPressed
         || event.type == sf::Event::MouseButtonPressed) {
            generate();
            return;
        }

    if (xpManager->canLevelUp()) {
        render::handleLevelupScreenEvent(*renderContext.window, *xpManager, event);
        return;
    }
       
    renderContext.camera->handleEvent(event);
    if (renderContext.camera->shouldStealControl())
        return;

    world->player().controller().handleEvent(event);
}

void Game::generate() {
    generationLogger->info("Started");

    generationLogger->info("Resetting world...");
    world->clearActors();
    world->clearItems();
    items->clearIdentifiedItems();
    items->randomizeTextures();
    world->tiles().assign({ 50, 50, 10 }, core::Tile::WALL);

    generationLogger->info("Generating dungeon...");
    dungeonGenerator()();

    generationLogger->info("Generating stairs...");
    world->generateStairs();

    actorSpawner->spawn();
    items->spawn();

    generationLogger->info("Finished");

    onGenerate();
}

void Game::draw_() {
    renderContext.window->clear();
    if (!world->player().isAlive()) {
        render::drawDeathScreen(*renderContext.window, *renderContext.assets);
    } else {
        render::draw(*renderContext.window, *renderContext.assets, *world, *renderContext.playerMap, renderContext.camera->position());
        renderContext.particles->draw(*renderContext.window, renderContext.camera->position());

        render::drawHud(*renderContext.window, *renderContext.assets, *world, *xpManager);
        if (xpManager->canLevelUp())
            render::drawLevelupScreen(*renderContext.window, *renderContext.assets, *xpManager);
    }
    renderContext.window->display();
}

void Game::save() const {
    saveLogger->info("Saving started...");

    std::unordered_multimap<std::string, std::string> multimap;

    saveLogger->info("Saving tiles...");
    multimap.emplace("Tiles", util::stringifyCharMap(world->tiles().transform(&core::charFromTile)));

    saveLogger->info("Saving stairs...");
    for (const auto& pair : world->upStairs()) {
        multimap.emplace("Stairs", JutchsON::write(pair));
    }

    saveLogger->info("Saving actors...");
    for (const auto& actor : world->actors()) {
        multimap.emplace("Actor", actorSpawner->stringifyActor(*actor));
    }

    saveLogger->info("Saving item textures...");
    multimap.emplace("ItemTextures", items->stringifyTextures());

    saveLogger->info("Saving items...");
    for (const auto& [position, item] : world->items()) {
        multimap.emplace("Item", JutchsON::write(ItemData{item->id(), item->stringifyData(), position}));
    }

    saveLogger->info("Saving known tiles...");
    multimap.emplace("KnownTiles", renderContext.playerMap->stringifyTileStates());

    saveLogger->info("Saving known actors...");
    for (const auto& actor : renderContext.playerMap->seenActors()) {
        multimap.emplace("KnownActor", renderContext.playerMap->stringifySeenActor(actor));
    }

    saveLogger->info("Saving known items...");
    for (auto item : renderContext.playerMap->seenItems()) {
        multimap.emplace("KnownItem", renderContext.playerMap->stringifySeenItem(item));
    }

    saveLogger->info("Saving sounds...");
    for (const auto& sound : renderContext.playerMap->recentSounds()) {
        multimap.emplace("Sound", sound.stringify());
    }

    saveLogger->info("Saving identified items...");
    multimap.emplace("IdentifiedItems", items->stringifyIdentifiedItems());

    saveLogger->info("Saving player leveling data...");
    multimap.emplace("Xp", xpManager->stringify());

    JutchsON::writeFile("latest.sav", multimap);

    saveLogger->info("Saving finished");
}
