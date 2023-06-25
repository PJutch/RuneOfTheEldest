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

#ifndef TILE_HPP_
#define TILE_HPP_

/// @file Tile.hpp Tile enum and its "methods"

/// Tile types
enum class Tile {
    EMPTY,         ///< Empty floor tile
    WALL,          ///< Impassable wall
    ROOM,          ///< Debug only @see BasicRoomGenerator::debugTiles
    ROOM_ENTRANCE, ///< Debug only @see BasicRoomGenerator::debugTiles
    PASSAGE,       ///< Debug only @see RandomSizeRoomGenerator::debugTiles
    DOWN_STAIRS,   ///< Stairs leading to next level
    UP_STAIRS,     ///< Stairs leading to previous level
    TOTAL_         ///< Technical enumerator. Should always be last
};

/// Total amount of tiles
const static int totalTiles = static_cast<int>(Tile::TOTAL_);

/// @brief Tests if Tile is passable (!= Tile::WALL)
/// @details Tile::UNSEEN is also impassable for placing player and debug dungeon generation 
[[nodiscard]] inline bool isPassable(Tile tile) {
    return tile != Tile::WALL;
}

/// same as isPassable
[[nodiscard]] inline bool isPassableTile(Tile tile) {
    return isPassable(tile);
}

/// @brief Tests if Tile is empty (Tile::EMPTY)
/// @details Debug tiles for marking generation cause are also empty
[[nodiscard]] inline bool isEmpty(Tile tile) {
    return tile == Tile::EMPTY 
        || tile == Tile::ROOM 
        || tile == Tile::ROOM_ENTRANCE 
        || tile == Tile::PASSAGE;
}

#endif