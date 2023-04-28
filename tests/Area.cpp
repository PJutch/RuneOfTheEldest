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

TEST(Area, splitXHorizontalPassages) {
    Area area{{1, 2, 5, 3}};
    area.addLeftPassage(3);
    area.addRightPassage(4);

    auto [left, right] = area.splitX(3);

    EXPECT_EQ(left.leftPassages().size(), 1);
    EXPECT_EQ(left.leftPassages()[0], 3);
    EXPECT_EQ(right.leftPassages().size(), 0);

    EXPECT_EQ(left.rightPassages().size(), 0);
    EXPECT_EQ(right.rightPassages().size(), 1);
    EXPECT_EQ(right.rightPassages()[0], 4);
}

TEST(Area, splitXVerticalPassages) {
    Area area{{1, 2, 5, 3}};
    area.addTopPassage(2);
    area.addTopPassage(5);
    area.addBottomPassage(2);
    area.addBottomPassage(4);

    auto [left, right] = area.splitX(3);

    EXPECT_EQ(left.topPassages().size(), 1);
    EXPECT_EQ(left.topPassages()[0], 2);
    EXPECT_EQ(right.topPassages().size(), 1);
    EXPECT_EQ(right.topPassages()[0], 5);

    EXPECT_EQ(left.bottomPassages().size(), 1);
    EXPECT_EQ(left.bottomPassages()[0], 2);
    EXPECT_EQ(right.bottomPassages().size(), 1);
    EXPECT_EQ(right.bottomPassages()[0], 4);
}

TEST(Area, splitYHorizontalPassages) {
    Area area{{1, 2, 5, 3}};
    area.addLeftPassage(3);
    area.addLeftPassage(6);
    area.addRightPassage(4);
    area.addRightPassage(6);

    auto [top, bottom] = area.splitY(5);

    EXPECT_EQ(top.leftPassages().size(), 1);
    EXPECT_EQ(top.leftPassages()[0], 3);
    EXPECT_EQ(bottom.leftPassages().size(), 1);
    EXPECT_EQ(bottom.leftPassages()[0], 6);

    EXPECT_EQ(top.rightPassages().size(), 1);
    EXPECT_EQ(top.rightPassages()[0], 4);
    EXPECT_EQ(bottom.rightPassages().size(), 1);
    EXPECT_EQ(bottom.rightPassages()[0], 6);
}

TEST(Area, splitYVerticalPassages) {
    Area area{{1, 2, 3, 5}};
    area.addTopPassage(2);
    area.addBottomPassage(3);

    auto [top, bottom] = area.splitY(5);

    EXPECT_EQ(top.topPassages().size(), 1);
    EXPECT_EQ(top.topPassages()[0], 2);
    EXPECT_EQ(bottom.topPassages().size(), 0);

    EXPECT_EQ(top.bottomPassages().size(), 0);
    EXPECT_EQ(bottom.bottomPassages().size(), 1);
    EXPECT_EQ(bottom.bottomPassages()[0], 3);
}
