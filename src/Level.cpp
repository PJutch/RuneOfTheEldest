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

#include "Level.hpp"

#include "Renderer.hpp"

void Level::generate(sf::Vector2i shape_, sf::IntRect room) {
    shape = shape_;
    tiles.resize(shape.x * shape.y, Tile::UNSEEN);

    for (int x = room.left; x < room.left + room.width; ++ x) {
        at(x, room.top) = Tile::WALL;
        at(x, room.top + room.height - 1) = Tile::WALL;
    }

    for (int y = room.top; y < room.top + room.height; ++ y) {
        at(room.left, y) = Tile::WALL;
        at(room.left + room.width - 1, y) = Tile::WALL;
    }

    for (int x = room.left + 1; x < room.left + room.width - 1; ++ x)
        for (int y = room.top + 1; y < room.top + room.height - 1; ++ y)
            at(x, y) = Tile::EMPTY;
}

void Level::draw(Renderer& renderer) const {
    for (int x = 0; x < shape.x; ++ x)
        for (int y = 0; y < shape.y; ++ y)
            renderer.draw(at(x, y), {x, y});
}
