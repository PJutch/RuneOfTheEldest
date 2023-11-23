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

#ifndef RANDOM_ROOM_GENERATOR_HPP_
#define RANDOM_ROOM_GENERATOR_HPP_

#include "RoomGenerator.hpp"
#include "BasicRoomGenerator.hpp"

#include "core/fwd.hpp"

#include "util/random.hpp"

#include <memory>

namespace generation {
    /// @brief Generates random sized room in the area
    /// @details Room generated by BasicRoomGenerator occupies random part of the area
    ///       \n Generates passages as corridors from room to sides of the area
    ///       \n Passages are grid aligned and turn <= 2 times
    /// @param If true generates special colored tiles instead of Tile::EMPTY
    ///       \n Generates Tile::PASSAGE in passages
    ///       \n Enables BasicRoomGenerator::debugTiles
    void randomSizeRoom(core::World& world, Area area, util::RandomEngine& randomEngine, bool debugTiles = false);
}

#endif