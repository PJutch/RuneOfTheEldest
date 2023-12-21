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

#ifndef LINE_HPP_
#define LINE_HPP_

#include "core/World.hpp"

namespace generation {
    void horizontalLine(core::World& world, int z, int left, int right, int y, core::Tile tile);
    void verticalLine(core::World& world, int z, int top, int bottom, int x, core::Tile tile);

    void horizontalLineInclusive(core::World& world, int z, int left, int right, int y, core::Tile tile);
    void verticalLineInclusive(core::World& world, int z, int top, int bottom, int x, core::Tile tile);
}

#endif