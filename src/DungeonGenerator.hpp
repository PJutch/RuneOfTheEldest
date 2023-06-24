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

#ifndef DUNGEON_GENERATOR_HPP_
#define DUNGEON_GENERATOR_HPP_

#include "RoomGenerator/RoomGenerator.hpp"
#include "Area.hpp"
#include "Level.hpp"

class Dungeon;

#include "random.hpp"

#include <SFML/Graphics.hpp>

#include <queue>
#include <functional>

/// Generates level using bsp
class DungeonGenerator {
public:
    DungeonGenerator(std::unique_ptr<RoomGenerator> roomGenerator, 
                     std::shared_ptr<Dungeon> dungeon,
                     RandomEngine& randomEngine);

    /// Minimum size of the room
    void minSize(int newMinSize) noexcept {
        minSize_ = newMinSize;
    }

    /// Chance of splitting room
    void splitChance(double newSplitChance) noexcept {
        splitChance_ = newSplitChance;
    }

    [[nodiscard]] RoomGenerator& roomGenerator() noexcept {
        return *roomGenerator_;
    }

    [[nodiscard]] const RoomGenerator& roomGenerator() const noexcept {
        return *roomGenerator_;
    }

    void operator() ();
private:
    std::unique_ptr<RoomGenerator> roomGenerator_;
    
    int minSize_ = 0;
    double splitChance_ = 0.0;

    std::queue<Area> areas;

    std::shared_ptr<Dungeon> dungeon;
    RandomEngine* randomEngine;

    void processLevel(Level& level);
    void processArea(Level& level, Area area);

    [[nodiscard]] bool canSplit(int dimension) const noexcept {
        return dimension > 2 * minSize_;
    }

    void splitX(Area area);
    void splitY(Area area);
};

#endif