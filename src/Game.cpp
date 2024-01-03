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

namespace {
    class UnknowSection : public util::RuntimeError {
    public:
        UnknowSection(std::string_view name, util::Stacktrace stacktrace = {}) : 
            util::RuntimeError{std::format("Unknown section \"{}\"", name), std::move(stacktrace)} {}
    };
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
    class ItemNotFound : public util::RuntimeError {
    public:
        ItemNotFound(std::string_view name, util::Stacktrace currentStacktrace = {}) noexcept :
            util::RuntimeError{std::format("Item \"{}\" not found", name), std::move(currentStacktrace)} {}
    };
}

void Game::loadFromString(std::string_view s) {
    saveLogger->info("Loading started...");

    util::KeyValueVisitor visitor;
    visitor.key("Tiles").unique().required().callback([&](std::string_view data) {
        saveLogger->info("Loading tiles...");
        world->tiles() = util::parseCharMap(data).transform(&core::tileFromChar);
    });
    visitor.key("Stairs").callback([&](std::string_view data) {
        auto [stairs1, stairs2] = util::parse2Vector3i(data);
        world->addStairs(stairs1, stairs2);
    });
    visitor.key("Actor").callback([&](std::string_view data) {
        auto actor = actorSpawner->parseActor(data);
        world->addActor(std::move(actor));
    });
    visitor.key("KnownTiles").unique().required().callback([&](std::string_view data) {
        saveLogger->info("Loading known tiles...");
        renderContext.playerMap->parseTileStates(data);
    });
    visitor.key("KnownActor").callback([&](std::string_view data) {
        renderContext.playerMap->parseSeenActor(data);
    });
    visitor.key("Sound").callback([&](std::string_view data) {
        renderContext.playerMap->addSound(core::Sound::parse(data));
    });
    visitor.key("Xp").unique().required().callback([&](std::string_view data) {
        saveLogger->info("Loading player leveling data...");
        xpManager->parse(data);
    });
    visitor.key("Item").callback([&](std::string_view s) {
        std::string_view id;
        std::string_view data;
        core::Position<int> position;

        util::KeyValueVisitor visitor;
        visitor.key("id").unique().required().callback([&](std::string_view s) {
            id = s;
        });
        visitor.key("data").unique().callback([&](std::string_view s) {
            data = s;
        });
        visitor.key("position").unique().required().callback([&](std::string_view s) {
            position = util::parsePositionInt(s);
        });

        util::forEackKeyValuePair(s, visitor);
        visitor.validate();

        if (auto item = items->findItem(id)) {
            auto newItem = item->clone();
            newItem->parseData(data);
            world->addItem(position, std::move(newItem));
        } else {
            throw ItemNotFound{id};
        }
    });
    visitor.key("KnownItem").callback([&](std::string_view data) {
        renderContext.playerMap->parseSeenItem(data);
    });

    util::forEachSection(s, visitor);
    visitor.validate();

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
    std::ofstream file{"latest.sav"};

    saveLogger->info("Saving tiles...");
    file << "[Tiles]\n" << util::stringifyCharMap(world->tiles().transform(&core::charFromTile));

    saveLogger->info("Saving stairs...");
    for (auto [upStairs, downStairs] : world->upStairs()) {
        file << "[Stairs]\n" << util::stringifyVector3(upStairs) << ' ' << util::stringifyVector3(downStairs) << '\n';
    }

    saveLogger->info("Saving actors...");
    for (const auto& actor : world->actors()) {
        file << "[Actor]\n" << actorSpawner->stringifyActor(*actor);
    }

    saveLogger->info("Saving items...");
    for (const auto& [position, item] : world->items()) {
        file << std::format("[Item]\nid {}\ndata {}\nposition {}\n", item->id(), item->stringifyData(), util::stringifyPosition(position));
    }

    saveLogger->info("Saving known tiles...");
    file << "[KnownTiles]\n" << renderContext.playerMap->stringifyTileStates();

    saveLogger->info("Saving known actors...");
    for (const auto& actor : renderContext.playerMap->seenActors()) {
        file << "[KnownActor]\n" << renderContext.playerMap->stringifySeenActor(actor);
    }

    saveLogger->info("Saving known items...");
    for (auto item : renderContext.playerMap->seenItems()) {
        file << "[KnownItem]\n" << renderContext.playerMap->stringifySeenItem(item);
    }

    saveLogger->info("Saving sounds...");
    for (const auto& sound : renderContext.playerMap->recentSounds()) {
        file << "[Sound]\n" << sound.stringify();
    }

    saveLogger->info("Saving player leveling data...");
    file << "[Xp]\n" << xpManager->stringify();

    saveLogger->info("Saving finished");
}
