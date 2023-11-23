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

#include "core/World.hpp"

#include "util/assert.hpp"

namespace generation {
    void basicRoom(core::World& world, Area area, bool debugTiles) {
        TROTE_ASSERT(world.tiles().isValidRect(area.bounds()));

        for (int x = area.left(); x < area.right() - 1; ++x)
            for (int y = area.top(); y < area.bottom() - 1; ++y)
                world.tiles()[{x, y, area.z()}] = (debugTiles ? Tile::ROOM
                    : Tile::EMPTY);

        for (int y : area.rightPassages())
            world.tiles()[{area.right() - 1, y, area.z()}] = (debugTiles
                ? Tile::ROOM_ENTRANCE : Tile::EMPTY);

        for (int x : area.bottomPassages())
            world.tiles()[{x, area.bottom() - 1, area.z()}] = (debugTiles
                ? Tile::ROOM_ENTRANCE : Tile::EMPTY);
    }
}
