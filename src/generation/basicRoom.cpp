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

#include "basicRoom.hpp"

#include "core/World.hpp"

#include "util/assert.hpp"

namespace generation {
    void rectRoom(core::World& world, Area area, bool debugTiles) {
        TROTE_ASSERT(world.tiles().isValidRect(area.bounds()));

        for (int x = area.left(); x < area.right() - 1; ++x)
            for (int y = area.top(); y < area.bottom() - 1; ++y)
                world.tiles()[{x, y, area.z()}] = (debugTiles ? core::Tile::ROOM
                    : core::Tile::EMPTY);

        for (int y : area.rightPassages())
            world.tiles()[{area.right() - 1, y, area.z()}] = (debugTiles
                ? core::Tile::ROOM_ENTRANCE : core::Tile::EMPTY);

        for (int x : area.bottomPassages())
            world.tiles()[{x, area.bottom() - 1, area.z()}] = (debugTiles
                ? core::Tile::ROOM_ENTRANCE : core::Tile::EMPTY);
    }

    void diamondRoom(core::World& world, Area area, bool debugTiles) {
        TROTE_ASSERT(world.tiles().isValidRect(area.bounds()));

        double xCenter = static_cast<double>(area.left() + area.right() - 1) / 2;
        double yCenter = static_cast<double>(area.top() + area.bottom() - 1) / 2;
        for (int x = area.left(); x < area.right() - 1; ++x)
            for (int y = area.top(); y < area.bottom() - 1; ++y) {
                double d = std::abs(x - xCenter) / (area.right() - xCenter)
                         + std::abs(y - yCenter) / (area.bottom() - yCenter);
                if (d <= 1)
                    world.tiles()[{x, y, area.z()}] = (debugTiles ? core::Tile::ROOM : core::Tile::EMPTY);
            }

        for (int y : area.leftPassages())
            for (int x = area.left(); x < area.right() - 1 && world.tiles()[{x, y, area.z()}] == core::Tile::WALL; ++x)
                world.tiles()[{x, y, area.z()}] = (debugTiles ? core::Tile::ROOM_ENTRANCE : core::Tile::EMPTY);

        for (int y : area.rightPassages())
            for (int x = area.right() - 1; x >= area.left() && world.tiles()[{x, y, area.z()}] == core::Tile::WALL; --x)
                world.tiles()[{x, y, area.z()}] = (debugTiles ? core::Tile::ROOM_ENTRANCE : core::Tile::EMPTY);

        for (int x : area.topPassages())
            for (int y = area.top(); y < area.bottom() - 1 && world.tiles()[{x, y, area.z()}] == core::Tile::WALL; ++y)
                world.tiles()[{x, y, area.z()}] = (debugTiles ? core::Tile::ROOM_ENTRANCE : core::Tile::EMPTY);

        for (int x : area.bottomPassages())
            for (int y = area.bottom() - 1; y >= area.top() && world.tiles()[{x, y, area.z()}] == core::Tile::WALL; --y)
                world.tiles()[{x, y, area.z()}] = (debugTiles ? core::Tile::ROOM_ENTRANCE : core::Tile::EMPTY);
    }

    void randomRoom(core::World& world, Area area, util::RandomEngine& engine, bool debugTiles) {
        const int minRandomRoomSize = 4;
        if (area.width() < minRandomRoomSize || area.height() < minRandomRoomSize) {
            rectRoom(world, area, debugTiles);
            return;
        }

        switch (std::uniform_int_distribution{0, 1}(engine)) {
        case 0: rectRoom(world, area, debugTiles); break;
        case 1: diamondRoom(world, area, debugTiles); break;
        default: TROTE_ASSERT(false, "Unreachable");
        }
    }
}
