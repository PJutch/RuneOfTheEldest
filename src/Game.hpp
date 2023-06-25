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

#ifndef GAME_HPP_
#define GAME_HPP_

#include "render/Renderer.hpp"
#include "generation/DungeonGenerator.hpp"

#include "util/random.hpp"
#include "util/log.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <memory>

/// Integrates all subsystems
class Game {
public:
    Game(std::shared_ptr<core::World> world,
         std::shared_ptr<core::Player> player,
         std::unique_ptr<generation::DungeonGenerator> dungeonGenerator,
         std::shared_ptr<sf::RenderWindow> window,
         std::shared_ptr<render::Camera> camera,
         std::shared_ptr<render::PlayerMap> playerMap,
         std::unique_ptr<render::Renderer> renderer,
         util::RandomEngine& randomEngine,
         util::LoggerFactory& loggerFactory);

    [[nodiscard]] render::Renderer& renderer() noexcept {
        return *renderer_;
    }

    [[nodiscard]] const render::Renderer& renderer() const noexcept {
        return *renderer_;
    }

    [[nodiscard]] generation::DungeonGenerator& dungeonGenerator() noexcept {
        return *dungeonGenerator_;
    }

    [[nodiscard]] const generation::DungeonGenerator& dungeonGenerator() const noexcept {
        return *dungeonGenerator_;
    }

    /// Generates world and runs game loop until exit
    void run();
private:
    std::shared_ptr<core::World> world;
    std::shared_ptr<core::Player> player;

    std::unique_ptr<generation::DungeonGenerator> dungeonGenerator_;

    std::shared_ptr<sf::RenderWindow> window;
    std::shared_ptr<render::Camera> camera;
    std::shared_ptr<render::PlayerMap> playerMap;
    std::unique_ptr<render::Renderer> renderer_;

    util::RandomEngine* randomEngine;
    std::shared_ptr<spdlog::logger> generationLogger;

    void handleEvent(sf::Event event);
    void generate();
};

#endif