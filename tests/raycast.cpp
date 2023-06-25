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
    World world;
    world.tiles().assign({ 3, 3, 1 }, Tile::EMPTY);

    EXPECT_TRUE(canSee({0, 2, 0}, {2, 1, 0}, world));
}

TEST(raycast, canSeeAdjenct) {
    World world;
    world.tiles().assign({ 2, 2, 1 }, Tile::WALL);

    world.tiles()[{0, 1, 0}] = Tile::EMPTY;
    world.tiles()[{1, 1, 0}] = Tile::EMPTY;

    EXPECT_TRUE(canSee({ 0, 1, 0 }, { 1, 1, 0 }, world));
}

TEST(raycast, canSeeAdjenctDiagonal) {
    World world;
    world.tiles().assign({ 2, 2, 1 }, Tile::WALL);

    world.tiles()[{0, 1, 0}] = Tile::EMPTY;
    world.tiles()[{1, 0, 0}] = Tile::EMPTY;

    EXPECT_TRUE(canSee({ 0, 1, 0 }, { 1, 0, 0 }, world));
}

TEST(raycast, canSeeBlockByWall) {
    World world;
    world.tiles().assign({ 3, 3, 1 }, Tile::WALL);

    world.tiles()[{0, 2, 0}] = Tile::EMPTY;
    world.tiles()[{2, 1, 0}] = Tile::EMPTY;

    EXPECT_FALSE(canSee({ 0, 2, 0 }, { 2, 1, 0 }, world));
}

TEST(raycast, canSeeBlockByFloor) {
    World world;
    world.tiles().assign({ 2, 2, 2 }, Tile::EMPTY);

    EXPECT_FALSE(canSee({ 0, 1, 0 }, { 1, 1, 1 }, world));
}

TEST(raycast, canSeeSelf) {
    World world;
    EXPECT_TRUE(canSee({1, 1, 1}, {1, 1, 1}, world));
}

TEST(raycast, canSeeWall) {
    World world;
    world.tiles().assign({ 3, 3, 1 }, Tile::EMPTY);
    world.tiles()[{ 2, 1, 0 }] = Tile::WALL;

    EXPECT_TRUE(canSee({ 0, 2, 0 }, { 2, 1, 0 }, world));
}
