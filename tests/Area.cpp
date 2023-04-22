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

#include "Area.hpp"

#include <gtest/gtest.h>

TEST(Area, right) {
    int right = Area{{2, 3, 7, 4}}.right();
    EXPECT_EQ(right, 9);
}

TEST(Area, bottom) {
    int bottom = Area{{2, 3, 7, 4}}.bottom();
    EXPECT_EQ(bottom, 7);
}

TEST(Area, splitX) {
    auto [area1, area2] = Area{{1, 2, 5, 3}}.splitX(3);

    EXPECT_EQ(area1.left(), 1);
    EXPECT_EQ(area1.top(), 2);
    EXPECT_EQ(area1.width(), 2);
    EXPECT_EQ(area1.height(), 3);

    EXPECT_EQ(area2.left(), 3);
    EXPECT_EQ(area2.top(), 2);
    EXPECT_EQ(area2.width(), 3);
    EXPECT_EQ(area2.height(), 3);
}

TEST(Area, splitY) {
    auto [area1, area2] = Area{{1, 2, 5, 3}}.splitY(3);

    EXPECT_EQ(area1.left(), 1);
    EXPECT_EQ(area1.top(), 2);
    EXPECT_EQ(area1.width(), 5);
    EXPECT_EQ(area1.height(), 1);

    EXPECT_EQ(area2.left(), 1);
    EXPECT_EQ(area2.top(), 3);
    EXPECT_EQ(area2.width(), 5);
    EXPECT_EQ(area2.height(), 2);
}