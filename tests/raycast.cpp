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

#include "raycast.hpp"

#include <gtest/gtest.h>

TEST(raycast, canSeeEmpty) {
    Dungeon dungeon;
    dungeon.resize(1);
    dungeon[0].generateBlank({ 3, 3 });
    
    for (int x = 0; x < 3; ++x)
        for (int y = 0; y < 3; ++y)
            dungeon.at(x, y, 0) = Tile::EMPTY;

    EXPECT_TRUE(canSee({0, 2, 0}, {2, 1, 0}, dungeon));
}

TEST(raycast, canSeeAdjenct) {
    Dungeon dungeon;
    dungeon.resize(1);
    dungeon[0].generateBlank({ 2, 2 });

    dungeon.at(0, 1, 0) = Tile::EMPTY;
    dungeon.at(1, 1, 0) = Tile::EMPTY;

    EXPECT_TRUE(canSee({ 0, 1, 0 }, { 1, 1, 0 }, dungeon));
}

TEST(raycast, canSeeAdjenctDiagonal) {
    Dungeon dungeon;
    dungeon.resize(1);
    dungeon[0].generateBlank({ 2, 2 });

    dungeon.at(0, 1, 0) = Tile::EMPTY;
    dungeon.at(1, 0, 0) = Tile::EMPTY;

    EXPECT_TRUE(canSee({ 0, 1, 0 }, { 1, 0, 0 }, dungeon));
}

TEST(raycast, canSeeBlockByWall) {
    Dungeon dungeon;
    dungeon.resize(1);
    dungeon[0].generateBlank({ 3, 3 });

    dungeon.at(0, 2, 0) = Tile::EMPTY;
    dungeon.at(2, 1, 0) = Tile::EMPTY;

    EXPECT_FALSE(canSee({ 0, 2, 0 }, { 2, 1, 0 }, dungeon));
}

TEST(raycast, canSeeBlockByFloor) {
    Dungeon dungeon;
    dungeon.resize(2);
    dungeon[0].generateBlank({ 2, 2 });
    dungeon[1].generateBlank({ 2, 2 });

    for (int x = 0; x < 2; ++x)
        for (int y = 0; y < 2; ++y)
            for (int z = 0; z < 2; ++z)
                dungeon.at(x, y, z) = Tile::EMPTY;

    EXPECT_FALSE(canSee({ 0, 1, 0 }, { 1, 1, 1 }, dungeon));
}

TEST(raycast, canSeeSelf) {
    Dungeon dungeon;
    EXPECT_TRUE(canSee({1, 1, 1}, {1, 1, 1}, dungeon));
}

TEST(raycast, canSeeWall) {
    Dungeon dungeon;
    dungeon.resize(1);
    dungeon[0].generateBlank({ 3, 3 });

    for (int x = 0; x < 3; ++x)
        for (int y = 0; y < 3; ++y)
            dungeon.at(x, y, 0) = Tile::EMPTY;
    dungeon.at({2, 1, 0}) = Tile::WALL;

    EXPECT_TRUE(canSee({ 0, 2, 0 }, { 2, 1, 0 }, dungeon));
}
