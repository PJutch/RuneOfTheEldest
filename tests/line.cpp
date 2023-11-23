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

#include "generation/line.hpp"

#include "core/World.hpp"

#include "util/geometry.hpp"

#include <gtest/gtest.h>

TEST(line, horizontalLine) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({3, 3, 1}, Tile::WALL);

    generation::horizontalLine(*world, 0, 1, 3, 1, Tile::EMPTY);

    EXPECT_EQ((world->tiles()[{0, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 1, 0}]), Tile::EMPTY);
    EXPECT_EQ((world->tiles()[{1, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 1, 0}]), Tile::EMPTY);
    EXPECT_EQ((world->tiles()[{2, 2, 0}]), Tile::WALL);
}

TEST(line, verticalLine) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({3, 3, 1}, Tile::WALL);

    generation::verticalLine(*world, 0, 1, 3, 1, Tile::EMPTY);

    EXPECT_EQ((world->tiles()[{0, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 1, 0}]), Tile::EMPTY);
    EXPECT_EQ((world->tiles()[{1, 2, 0}]), Tile::EMPTY);
    EXPECT_EQ((world->tiles()[{2, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 2, 0}]), Tile::WALL);
}

TEST(line, horizontalLineSwap) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({3, 3, 1}, Tile::WALL);

    generation::horizontalLine(*world, 0, 3, 1, 1, Tile::EMPTY);

    EXPECT_EQ((world->tiles()[{0, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 1, 0}]), Tile::EMPTY);
    EXPECT_EQ((world->tiles()[{1, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 1, 0}]), Tile::EMPTY);
    EXPECT_EQ((world->tiles()[{2, 2, 0}]), Tile::WALL);
}

TEST(line, verticalLineSwap) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({3, 3, 1}, Tile::WALL);

    generation::verticalLine(*world, 0, 3, 1, 1, Tile::EMPTY);

    EXPECT_EQ((world->tiles()[{0, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 1, 0}]), Tile::EMPTY);
    EXPECT_EQ((world->tiles()[{1, 2, 0}]), Tile::EMPTY);
    EXPECT_EQ((world->tiles()[{2, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 2, 0}]), Tile::WALL);
}

TEST(line, horizontalLine1) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({3, 3, 1}, Tile::WALL);

    generation::horizontalLine(*world, 0, 1, 2, 1, Tile::EMPTY);

    EXPECT_EQ((world->tiles()[{0, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 1, 0}]), Tile::EMPTY);
    EXPECT_EQ((world->tiles()[{1, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 2, 0}]), Tile::WALL);
}

TEST(line, verticalLine1) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({3, 3, 1}, Tile::WALL);

    generation::verticalLine(*world, 0, 1, 2, 1, Tile::EMPTY);

    EXPECT_EQ((world->tiles()[{0, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 1, 0}]), Tile::EMPTY);
    EXPECT_EQ((world->tiles()[{1, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 2, 0}]), Tile::WALL);
}

TEST(line, horizontalLine0) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({3, 3, 1}, Tile::WALL);

    generation::horizontalLine(*world, 0, 1, 1, 1, Tile::EMPTY);

    EXPECT_EQ((world->tiles()[{0, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 2, 0}]), Tile::WALL);
}

TEST(line, verticalLine0) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({3, 3, 1}, Tile::WALL);

    generation::verticalLine(*world, 0, 1, 1, 1, Tile::EMPTY);

    EXPECT_EQ((world->tiles()[{0, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{0, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{1, 2, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 0, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 1, 0}]), Tile::WALL);
    EXPECT_EQ((world->tiles()[{2, 2, 0}]), Tile::WALL);
}
