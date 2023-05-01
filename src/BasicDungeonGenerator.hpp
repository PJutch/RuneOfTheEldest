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

#ifndef BASIC_DUNGEON_GENERATOR_HPP_
#define BASIC_DUNGEON_GENERATOR_HPP_

#include "RoomGenerator/RoomGenerator.hpp"
#include "Area.hpp"

#include "random.hpp"

#include <SFML/Graphics.hpp>

#include <queue>
#include <functional>

class BasicDungeonGenerator {
public:
    BasicDungeonGenerator(std::unique_ptr<RoomGenerator> roomGenerator,
        RandomEngine& randomEngine);

    void minSize(int newMinSize) noexcept {
        minSize_ = newMinSize;
    }

    void splitChance(double newSplitChance) noexcept {
        splitChance_ = newSplitChance;
    }

    void startRect(sf::IntRect newStartRect) noexcept {
        startRect_ = newStartRect;
    }

    void operator() ();
private:
    std::unique_ptr<RoomGenerator> roomGenerator;
    
    int minSize_;
    double splitChance_;
    sf::IntRect startRect_;

    std::queue<Area> areas;

    RandomEngine* randomEngine;

    void processArea(Area area);

    bool canSplit(int dimension) const noexcept {
        return dimension > 2 * minSize_;
    }

    void splitX(Area area);
    void splitY(Area area);
};

#endif