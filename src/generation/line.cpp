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

#include "line.hpp"

#include <utility>

namespace generation {
    void horizontalLine(core::World& world, int z, int left, int right, int y, core::Tile tile) {
        using std::swap;
        if (left > right)
            swap(left, right);

        for (int x = left; x < right; ++x)
            world.tiles()[{x, y, z}] = tile;
    }

    void verticalLine(core::World& world, int z, int top, int bottom, int x, core::Tile tile) {
        using std::swap;
        if (top > bottom)
            swap(top, bottom);

        for (int y = top; y < bottom; ++y)
            world.tiles()[{x, y, z}] = tile;
    }

    void horizontalLineInclusive(core::World& world, int z, int left, int right, int y, core::Tile tile) {
        using std::swap;
        if (left > right)
            swap(left, right);

        horizontalLine(world, z, left, right + 1, y, tile);
    }

    void verticalLineInclusive(core::World& world, int z, int top, int bottom, int x, core::Tile tile) {
        using std::swap;
        if (top > bottom)
            swap(top, bottom);

        verticalLine(world, z, top, bottom + 1, x, tile);
    }
}
