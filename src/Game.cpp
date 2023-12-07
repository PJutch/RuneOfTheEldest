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

#include "util/Keyboard.hpp"
#include "util/raycast.hpp"

Game::Game(std::shared_ptr<core::World> newWorld,
           std::unique_ptr<core::ActorSpawner> actorSpawner_,
           std::shared_ptr<core::XpManager> xpManager_,
           std::unique_ptr<generation::DungeonGenerator> newDungeonGenerator,
           render::Context renderContext_,
           std::shared_ptr<util::Raycaster> raycaster,
           util::LoggerFactory& loggerFactory) :
        world{std::move(newWorld)},
        actorSpawner{std::move(actorSpawner_)},
        xpManager{std::move(xpManager_)},
        dungeonGenerator_{std::move(newDungeonGenerator)},
        renderContext{std::move(renderContext_)},
        generationLogger{ loggerFactory.create("generation") } {
    addOnGenerateListener([camera = renderContext.camera]() { camera->reset(); });
    addOnGenerateListener([playerMap = renderContext.playerMap]() { playerMap->onGenerate(); });
    addOnGenerateListener([particles = renderContext.particles]() { particles->clear(); });
    addOnGenerateListener([raycaster = std::move(raycaster)]() { raycaster->clear(); });
    addOnGenerateListener([xpManager = xpManager]() { xpManager->onGenerate(); });

    addOnUpdateListener([camera = renderContext.camera](sf::Time elapsedTime) { camera->update(elapsedTime); });
    addOnUpdateListener([playerMap = renderContext.playerMap](sf::Time) { playerMap->update(); });
    addOnUpdateListener([particles = renderContext.particles](sf::Time elapsedTime) { particles->update(elapsedTime); });
}

void Game::run() {
    generate();

    sf::Clock clock;
	while (renderContext.window->isOpen()) {
        sf::Event event;
        while (renderContext.window->pollEvent(event))
            handleEvent(event);

        if (world->player().isAlive() && !xpManager->canLevelUp()) {
            world->update();

            sf::Time elapsedTime = clock.restart();
            onUpdate(elapsedTime);
        }

        draw_();
    }
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

    world->clearActors();
    world->tiles().assign({ 50, 50, 10 }, Tile::WALL);

    generationLogger->info("Generating dungeon...");
    dungeonGenerator()();

    generationLogger->info("Generating stairs...");
    world->generateStairs();

    actorSpawner->spawn();

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
