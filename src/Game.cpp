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

#include "Event.hpp"

Game::Game(std::shared_ptr<World> newWorld,
           std::shared_ptr<Player> player_,
           std::shared_ptr<sf::RenderWindow> window_,
           std::shared_ptr<Camera> camera_,
           std::unique_ptr<Renderer> newRenderer,
           RandomEngine& randomEngine_,
           LoggerFactory& loggerFactory) :
    world_{std::move(newWorld)},
    player{std::move(player_)},
    window{std::move(window_)},
    camera{std::move(camera_)},
    renderer_{std::move(newRenderer)},
    generationLogger{ loggerFactory.create("generation") },
    randomEngine{ &randomEngine_ } {}

void Game::run() {
    generate();

    sf::Clock clock;
	while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event))
            handleEvent(event);

        sf::Time elapsedTime = clock.restart();
        camera->update(elapsedTime);

        renderer().draw();
    }
}

void Game::handleEvent(sf::Event event) {
    if (event.type == sf::Event::Closed
     || wasKeyPressed(event, sf::Keyboard::Escape)) {
        window->close();
        return;
    }
       
    camera->handleEvent(event);

    if (!camera->shouldStealControl())
        player->handleEvent(event);
}

void Game::generate() {
    generationLogger->info("Started");
    world_->generate(generationLogger);

    generationLogger->info("Placing player...");
    player->level(0);
    player->position((*world_)[player->level()].randomEmptyPosition(*randomEngine));

    generationLogger->info("Finished");
}
