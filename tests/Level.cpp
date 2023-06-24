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

#include <gtest/gtest.h>

#include <array>

TEST(Level, generateBlankShape) {
    sf::Vector2i shape{10, 7};

    Level level;
    level.generateBlank(shape);

    EXPECT_EQ(level.shape(), shape);
}

TEST(Level, generateBlank) {
    Level level;
    level.generateBlank({10, 7});

    for (int x = 0; x < level.shape().x; ++ x)
        for (int y = 0; y < level.shape().y; ++ y)
            EXPECT_EQ(level.at(x, y), Tile::WALL);
}
