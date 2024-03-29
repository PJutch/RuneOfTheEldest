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

#ifndef BASIC_ROOM_GENERATOR_HPP_
#define BASIC_ROOM_GENERATOR_HPP_

#include "Area.hpp"

#include "core/fwd.hpp"
#include "util/random.hpp"

#include <memory>

namespace generation {
    /// @brief Generates rectangular room in the area
    /// @details Room occupies all area except right and bottom sides
    ///       \n Generates Tile::EMPTY for passages in right and bottom sides
    /// @param debugTiles If true generates special colored tiles instead of Tile::EMPTY
    ///       \n Generates Tile::ROOM in main room
    ///       \n Generates Tile::ROOM_ENTRANCE for passage entrance
    void rectRoom(core::World& world, Area area, bool debugTiles = false);

    /// @brief Generates diamond shaped room in the area
    /// @details Room occupies all area except right and bottom sides
    ///       \n Generates Tile::EMPTY lines for passages
    /// @param debugTiles If true generates special colored tiles instead of Tile::EMPTY
    ///       \n Generates Tile::ROOM in main room
    ///       \n Generates Tile::ROOM_ENTRANCE for passage entrance
    void diamondRoom(core::World& world, Area area, bool debugTiles = false);

    /// Choose one of room types randomly
    void randomRoom(core::World& world, Area area, util::RandomEngine& engine, bool debugTiles = false);
}

#endif