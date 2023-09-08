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

#include "core/fwd.hpp"
#include "render/fwd.hpp"
#include "generation/fwd.hpp"

#include "util/random.hpp"
#include "util/log.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

/// Integrates all subsystems
class Game {
public:
    Game(std::shared_ptr<core::World> world,
         std::unique_ptr<core::ActorSpawner> enemySpawner,
         std::shared_ptr<core::XpManager> xpManager,
         std::unique_ptr<generation::DungeonGenerator> dungeonGenerator,
         std::shared_ptr<sf::RenderWindow> window,
         std::unique_ptr<render::Renderer> renderer,
         std::shared_ptr<render::Camera> camera,
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
    std::unique_ptr<core::ActorSpawner> actorSpawner;
    std::shared_ptr<core::XpManager> xpManager;

    std::unique_ptr<generation::DungeonGenerator> dungeonGenerator_;

    std::shared_ptr<sf::RenderWindow> window;
    std::unique_ptr<render::Renderer> renderer_;
    std::shared_ptr<render::Camera> camera;

    std::shared_ptr<spdlog::logger> generationLogger;

    void handleEvent(sf::Event event);
    void generate();
    void draw_();
};

#endif