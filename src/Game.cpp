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

#include "Player.hpp"
#include "Goblin.hpp"

#include "util/Keyboard.hpp"

Game::Game(std::shared_ptr<World> newWorld,
           std::shared_ptr<Player> player_,
           std::unique_ptr<generation::DungeonGenerator> newDungeonGenerator,
           std::shared_ptr<sf::RenderWindow> window_,
           std::shared_ptr<render::Camera> camera_,
           std::shared_ptr<render::PlayerMap> playerMap_,
           std::unique_ptr<render::Renderer> newRenderer,
           util::RandomEngine& randomEngine_,
           util::LoggerFactory& loggerFactory) :
    world{std::move(newWorld)},
    player{ std::move(player_) },
    dungeonGenerator_{std::move(newDungeonGenerator)},
    window{std::move(window_)},
    camera{std::move(camera_)},
    playerMap{ std::move(playerMap_) },
    renderer_{std::move(newRenderer)},
    randomEngine{ &randomEngine_ }, 
    generationLogger{ loggerFactory.create("generation") } {}

void Game::run() {
    generate();

    sf::Clock clock;
	while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event))
            handleEvent(event);

        if (player->isAlive()) {
            world->update();

            sf::Time elapsedTime = clock.restart();
            renderer().update(elapsedTime);

            renderer().draw();
        } else
            renderer().drawDeathScreen();
    }
}

void Game::handleEvent(sf::Event event) {
    if (event.type == sf::Event::Closed
     || util::wasKeyPressed(event, sf::Keyboard::Escape)) {
        window->close();
        return;
    }

    if (!player->isAlive()) {
        if (event.type == sf::Event::KeyPressed
         || event.type == sf::Event::MouseButtonPressed) {
            camera->reset();
            generate();
            return;
        }
    }
       
    camera->handleEvent(event);

    if (!camera->shouldStealControl())
        player->handleEvent(event);
}

void Game::generate() {
    generationLogger->info("Started");

    world->clearActors();
    world->tiles().assign({ 50, 50, 10 }, Tile::WALL);

    generationLogger->info("Generating dungeon...");
    dungeonGenerator()();

    generationLogger->info("Generating stairs...");
    world->generateStairs();

    generationLogger->info("Spawning goblins...");
    Goblin::spawnAll(world, player, *randomEngine);

    generationLogger->info("Spawning player...");
    player->spawn();

    generationLogger->info("Finished");

    playerMap->onGenerate();
}
