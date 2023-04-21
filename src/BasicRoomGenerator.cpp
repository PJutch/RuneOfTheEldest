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

#include "BasicRoomGenerator.hpp"

void BasicRoomGenerator::operator() (sf::IntRect room) {
    std::shared_ptr<Level> level_ = level.lock();

    for (int x = room.left - 1; x < room.left + room.width; ++ x) {
        level_->at(x, room.top - 1) = Level::Tile::WALL;
        level_->at(x, room.top + room.height - 1) = Level::Tile::WALL;
    }

    for (int y = room.top - 1; y < room.top + room.height; ++ y) {
        level_->at(room.left - 1, y) = Level::Tile::WALL;
        level_->at(room.left + room.width - 1, y) = Level::Tile::WALL;
    }

    for (int x = room.left; x < room.left + room.width - 1; ++ x)
        for (int y = room.top; y < room.top + room.height - 1; ++ y)
            level_->at(x, y) = Level::Tile::EMPTY;
}
