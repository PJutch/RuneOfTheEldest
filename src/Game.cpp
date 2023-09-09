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

#include "render/World.hpp"
#include "render/DeathScreen.hpp"
#include "render/Hud.hpp"
#include "render/LevelUpScreen.hpp"
#include "render/Camera/Camera.hpp"
#include "render/View.hpp"

#include "generation/DungeonGenerator.hpp"

#include "core/World.hpp"
#include "core/ActorSpawner.hpp"

#include "util/Keyboard.hpp"

Game::Game(std::shared_ptr<core::World> newWorld,
           std::unique_ptr<core::ActorSpawner> actorSpawner_,
           std::shared_ptr<core::XpManager> xpManager_,
           std::unique_ptr<generation::DungeonGenerator> newDungeonGenerator,
           std::shared_ptr<sf::RenderWindow> window_,
           std::shared_ptr<render::AssetManager> assets_,
           std::shared_ptr<render::Camera> camera_,
           std::shared_ptr<render::PlayerMap> playerMap_,
           util::LoggerFactory& loggerFactory) :
    world{std::move(newWorld)},
    actorSpawner{std::move(actorSpawner_)},
    xpManager{std::move(xpManager_)},
    dungeonGenerator_{std::move(newDungeonGenerator)},
    window{std::move(window_)},
    assets{std::move(assets_)}, 
    camera{std::move(camera_)}, playerMap{std::move(playerMap_)},
    generationLogger{ loggerFactory.create("generation") } {}

void Game::run() {
    generate();

    sf::Clock clock;
	while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event))
            handleEvent(event);

        if (world->player().isAlive() && !xpManager->canLevelUp()) {
            world->update();

            sf::Time elapsedTime = clock.restart();
            camera->update(elapsedTime);
            playerMap->update();
        }

        draw_();
    }
}

void Game::handleEvent(sf::Event event) {
    if (event.type == sf::Event::Closed
     || util::wasKeyPressed(event, sf::Keyboard::Escape)) {
        window->close();
        return;
    }

    if (!world->player().isAlive())
        if (event.type == sf::Event::KeyPressed
         || event.type == sf::Event::MouseButtonPressed) {
            generate();
            return;
        }

    if (xpManager->canLevelUp()) {
        render::handleLevelupScreenEvent(*window, *xpManager, event);
        return;
    }
       
    camera->handleEvent(event);
    if (camera->shouldStealControl())
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

    camera->reset();
    playerMap->onGenerate();
    xpManager->onGenerate();
}

void Game::draw_() {
    window->clear();
    if (!world->player().isAlive()) {
        render::drawDeathScreen(*window, *assets);
    } else {
        window->setView(render::worldScreenView(camera->position().xy(), window->getSize()));
        render::draw(*window, *assets, *world, *playerMap, camera->position().level);

        window->setView(render::hudView(window->getSize()));
        render::drawXpBar(*window, *xpManager);
        if (xpManager->canLevelUp())
            render::drawLevelupScreen(*window, *assets, *world, *xpManager);
    }
    window->display();
}
