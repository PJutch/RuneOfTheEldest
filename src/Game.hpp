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
#include "generation/fwd.hpp"

#include "render/Context.hpp"

#include "util/raycast.hpp"
#include "util/Signal.hpp"
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
         render::Context renderContext,
         std::shared_ptr<util::Raycaster> raycaster,
         util::LoggerFactory& loggerFactory);

    [[nodiscard]] generation::DungeonGenerator& dungeonGenerator() noexcept {
        return *dungeonGenerator_;
    }

    [[nodiscard]] const generation::DungeonGenerator& dungeonGenerator() const noexcept {
        return *dungeonGenerator_;
    }

    /// Generates world and runs game loop until exit
    void run();

    void addOnGenerateListener(auto listener) {
        onGenerate.addListener(std::move(listener));
    }

    void addOnUpdateListener(auto listener) {
        onUpdate.addListener(std::move(listener));
    }
private:
    std::shared_ptr<core::World> world;
    std::unique_ptr<core::ActorSpawner> actorSpawner;
    std::shared_ptr<core::XpManager> xpManager;

    std::unique_ptr<generation::DungeonGenerator> dungeonGenerator_;

    render::Context renderContext;

    std::shared_ptr<spdlog::logger> generationLogger;
    std::shared_ptr<spdlog::logger> saveLogger;

    util::Signal<> onGenerate;
    util::Signal<sf::Time> onUpdate;

    void handleEvent(sf::Event event);
    void generate();
    void draw_();

    void loadFromString(std::string_view s);
    void save() const;
};

#endif