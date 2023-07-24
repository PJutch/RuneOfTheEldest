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

#include "core/ActorSpawner.hpp"

#include "generation/RandomSizeRoomGenerator.hpp"
#include "generation/DungeonGenerator.hpp"

#include "render/Renderer.hpp"
#include "render/FreeCamera.hpp"
#include "render/PlayerLockedCamera.hpp"
#include "render/SwitchableCamera.hpp"

#include "util/log.hpp"
#include "util/Exception.hpp"
#include "util/random.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <SFML/System/String.hpp>

#include <boost/di.hpp>

#include <cstdlib>

sf::String createSfString(std::string_view string) {
    return sf::String::fromUtf8(std::ranges::begin(string), 
                                std::ranges::end(string));
}

int main() {
    auto logModule_ = util::logModule();
    auto loggerFactory = logModule_.create<util::LoggerFactory>();
    auto logger = loggerFactory.create("main");   

    try {
        logger->info("Loading...");

        auto videoMode = sf::VideoMode::getDesktopMode();
        auto renderWindow = std::make_shared<sf::RenderWindow>(
            videoMode, 
            createSfString("The Rune of the Eldest"), 
            sf::Style::Fullscreen
        );
        renderWindow->setVerticalSyncEnabled(true);

        util::SeedT seed = std::random_device{}();
        util::RandomEngine randomEngine{seed};
        logger->info("Random seed is {}", seed);

        auto injector = boost::di::make_injector(
            std::move(logModule_),
            boost::di::bind<util::LoggerFactory>.to(loggerFactory),
            boost::di::bind<sf::VideoMode>.to(videoMode),
            boost::di::bind<sf::RenderWindow>.to(renderWindow),
            boost::di::bind<util::RandomEngine>.to(randomEngine),
            boost::di::bind<generation::RoomGenerator>.to<generation::RandomSizeRoomGenerator>(),
            boost::di::bind<render::Camera*[]>.to<render::PlayerLockedCamera, render::FreeCamera>(),
            boost::di::bind<render::Camera>.to<render::SwitchableCamera>()
        );
        auto game = injector.create<Game>();

        game.dungeonGenerator().splitChance(0.9);
        game.dungeonGenerator().minSize(5);
        game.renderer().playerMap().seeEverything(false);
        // game.dungeonGenerator().roomGenerator().debugTiles();
        // game.renderer().renderAreas();

        logger->info("Loading complete");
        game.run();
        logger->info("Exiting...");
    } catch (const util::TracableException& e) {
        logger->critical("Error occured: {}\nStacktrace:\n{}", 
                         e.what(), e.stacktrace());
    } catch (const std::exception& e) {
        logger->critical("Error occured: {}", e.what());
    }

    logger->info("Exited");

    return EXIT_SUCCESS;
}
