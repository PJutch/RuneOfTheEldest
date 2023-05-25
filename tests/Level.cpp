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
            EXPECT_EQ(level.at(x, y), Tile::UNSEEN);
}

TEST(Level, generateWalls) {
    const int shapeX = 7;
    const int shapeY = 5;

    Level level;
    level.generateBlank({shapeX, shapeY});

    level.at(2, 2) = Tile::EMPTY;
    level.at(3, 2) = Tile::EMPTY;

    level.generateWalls();

    using enum Tile;
    std::array<std::array<Tile, shapeX>, shapeY> result;
    result[0] = {UNSEEN, UNSEEN, UNSEEN, UNSEEN, UNSEEN, UNSEEN, UNSEEN};
    result[1] = {UNSEEN, WALL,   WALL,   WALL,   WALL,   UNSEEN, UNSEEN};
    result[2] = {UNSEEN, WALL,   EMPTY,  EMPTY,  WALL,   UNSEEN, UNSEEN};
    result[3] = {UNSEEN, WALL,   WALL,   WALL,   WALL,   UNSEEN, UNSEEN};
    result[4] = {UNSEEN, UNSEEN, UNSEEN, UNSEEN, UNSEEN, UNSEEN, UNSEEN};

    for (int x = 0; x < level.shape().x; ++ x)
        for (int y = 0; y < level.shape().y; ++ y)
            EXPECT_EQ(level.at(x, y), result[y][x]);
}

TEST(Level, generateWalls1x1) {
    const int shapeX = 5;
    const int shapeY = 5;

    Level level;
    level.generateBlank({shapeX, shapeY});

    level.at(2, 2) = Tile::EMPTY;

    level.generateWalls();

    using enum Tile;
    std::array<std::array<Tile, shapeX>, shapeY> result;
    result[0] = {UNSEEN, UNSEEN, UNSEEN, UNSEEN, UNSEEN};
    result[1] = {UNSEEN, WALL,   WALL,   WALL,   UNSEEN};
    result[2] = {UNSEEN, WALL,   EMPTY,  WALL,   UNSEEN};
    result[3] = {UNSEEN, WALL,   WALL,   WALL,   UNSEEN};
    result[4] = {UNSEEN, UNSEEN, UNSEEN, UNSEEN, UNSEEN};

    for (int x = 0; x < level.shape().x; ++ x)
        for (int y = 0; y < level.shape().y; ++ y)
            EXPECT_EQ(level.at(x, y), result[y][x]);
}

TEST(Level, generateWallsNearEdge) {
    const int shapeX = 5;
    const int shapeY = 5;

    Level level;
    level.generateBlank({shapeX, shapeY});

    level.at(1, 2) = Tile::EMPTY;

    level.generateWalls();

    using enum Tile;
    std::array<std::array<Tile, shapeX>, shapeY> result;
    result[0] = {UNSEEN, UNSEEN, UNSEEN, UNSEEN, UNSEEN};
    result[1] = {WALL,   WALL,   WALL,   UNSEEN, UNSEEN};
    result[2] = {WALL,   EMPTY,  WALL,   UNSEEN, UNSEEN};
    result[3] = {WALL,   WALL,   WALL,   UNSEEN, UNSEEN};
    result[4] = {UNSEEN, UNSEEN, UNSEEN, UNSEEN, UNSEEN};

    for (int x = 0; x < level.shape().x; ++ x)
        for (int y = 0; y < level.shape().y; ++ y)
            EXPECT_EQ(level.at(x, y), result[y][x]);
}

TEST(Level, generateWallsOnEdge) {
    const int shapeX = 3;
    const int shapeY = 5;

    Level level;
    level.generateBlank({shapeX, shapeY});

    level.at(0, 2) = Tile::EMPTY;

    level.generateWalls();

    using enum Tile;
    std::array<std::array<Tile, shapeX>, shapeY> result;
    result[0] = {UNSEEN, UNSEEN, UNSEEN};
    result[1] = {WALL,   WALL,   UNSEEN};
    result[2] = {EMPTY,  WALL,   UNSEEN};
    result[3] = {WALL,   WALL,   UNSEEN};
    result[4] = {UNSEEN, UNSEEN, UNSEEN};

    for (int x = 0; x < level.shape().x; ++ x)
        for (int y = 0; y < level.shape().y; ++ y)
            EXPECT_EQ(level.at(x, y), result[y][x]);
}
