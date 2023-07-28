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

#include "util/raycast.hpp"

#include "core/World.hpp"

#include <gtest/gtest.h>

TEST(raycast, canSeeEmpty) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 3, 3, 1 }, Tile::EMPTY);

    util::Raycaster raycaster{ std::move(world) };
    EXPECT_TRUE(raycaster.canSee({0, 2, 0}, {2, 1, 0}));
}

TEST(raycast, canSeeAdjenct) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 2, 2, 1 }, Tile::WALL);

    world->tiles()[{0, 1, 0}] = Tile::EMPTY;
    world->tiles()[{1, 1, 0}] = Tile::EMPTY;

    util::Raycaster raycaster{ std::move(world) };
    EXPECT_TRUE(raycaster.canSee({ 0, 1, 0 }, { 1, 1, 0 }));
}

TEST(raycast, canSeeAdjenctDiagonal) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 2, 2, 1 }, Tile::WALL);

    world->tiles()[{0, 1, 0}] = Tile::EMPTY;
    world->tiles()[{1, 0, 0}] = Tile::EMPTY;

    util::Raycaster raycaster{ std::move(world) };
    EXPECT_TRUE(raycaster.canSee({ 0, 1, 0 }, { 1, 0, 0 }));
}

TEST(raycast, canSeeBlockByWall) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 3, 3, 1 }, Tile::WALL);

    world->tiles()[{0, 2, 0}] = Tile::EMPTY;
    world->tiles()[{2, 1, 0}] = Tile::EMPTY;

    util::Raycaster raycaster{ std::move(world) };
    EXPECT_FALSE(raycaster.canSee({ 0, 2, 0 }, { 2, 1, 0 }));
}

TEST(raycast, canSeeBlockByFloor) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 2, 2, 2 }, Tile::EMPTY);

    util::Raycaster raycaster{ std::move(world) };
    EXPECT_FALSE(raycaster.canSee({ 0, 1, 0 }, { 1, 1, 1 }));
}

TEST(raycast, canSeeSelf) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 2, 2, 2 }, Tile::WALL);
    world->tiles()[{1, 1, 1}] = Tile::EMPTY;

    util::Raycaster raycaster{ std::move(world) };
    EXPECT_TRUE(raycaster.canSee({1, 1, 1}, {1, 1, 1}));
}

TEST(raycast, canSeeWall) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 3, 3, 1 }, Tile::EMPTY);
    world->tiles()[{ 2, 1, 0 }] = Tile::WALL;

    util::Raycaster raycaster{ std::move(world) };
    EXPECT_TRUE(raycaster.canSee({ 0, 2, 0 }, { 2, 1, 0 }));
}

TEST(raycast, canSeeWallStiffAngle) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 2, 5, 1 }, Tile::EMPTY);
    world->tiles()[{ 1, 0, 0 }] = Tile::WALL;
    world->tiles()[{ 1, 1, 0 }] = Tile::WALL;
    world->tiles()[{ 1, 2, 0 }] = Tile::WALL;
    world->tiles()[{ 1, 3, 0 }] = Tile::WALL;
    world->tiles()[{ 1, 4, 0 }] = Tile::WALL;

    util::Raycaster raycaster{ std::move(world) };
    EXPECT_TRUE(raycaster.canSee({ 0, 4, 0 }, { 1, 0, 0 }));
}
