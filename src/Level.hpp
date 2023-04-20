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

#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include "random.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <vector>

class Level{
public:
    Level(RandomEngine& randomEngine_) : randomEngine{&randomEngine_} {}

    enum class Tile {
        EMPTY,
        WALL,
        UNSEEN,
        TOTAL_
    };

    const static int totalTiles = static_cast<int>(Tile::TOTAL_);

    // unsafe
    Tile& at(int x, int y) noexcept {
        return tiles[x * shape().y + y];
    }

    // unsafe
    const Tile& at(int x, int y) const noexcept {
        return tiles[x * shape().y + y];
    }

    void generate(sf::Vector2i shape_, sf::IntRect room);

    sf::Vector2i shape() const noexcept {
        return shape_;
    }
private:
    sf::Vector2i shape_;
    std::vector<Tile> tiles;

    RandomEngine* randomEngine;

    void generateBlank(sf::Vector2i shape);
    void generateDungeon();
    void generateRoom(sf::IntRect room);
};

#endif