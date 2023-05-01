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

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <span>

class Level{
public:
    enum class Tile {
        EMPTY,
        WALL,
        UNSEEN,
        ROOM,
        ROOM_ENTRANCE,
        PASSAGE,
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

    sf::Vector2i shape() const noexcept {
        return shape_;
    }

    sf::IntRect bounds() const noexcept {
        return {{0, 0}, shape()};
    }

    void generateBlank(sf::Vector2i shape);
    void generateWalls();

    void addArea(sf::IntRect area) {
        areas_.push_back(area);
    }

    std::span<const sf::IntRect> areas() const noexcept {
        return areas_;
    }
private:
    sf::Vector2i shape_;
    std::vector<Tile> tiles;

    std::vector<sf::IntRect> areas_;
};

#endif