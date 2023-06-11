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

#include "pathfinding.hpp"

#include <gtest/gtest.h>

TEST(pathfinding, nextStepInPlace) {
    Dungeon dungeon;
    dungeon.resize(1);
    dungeon[0].generateBlank({ 3, 3 });
    dungeon.at(0, 1, 0) = Tile::EMPTY;

    EXPECT_EQ(nextStep(dungeon, {0, 1, 0}, {0, 1, 0}), (sf::Vector3i{0, 0, 0}));
}

TEST(pathfinding, nextStepFree) {
    Dungeon dungeon;
    dungeon.resize(1);
    dungeon[0].generateBlank({ 3, 3 });
    for (int x = 0; x < 3; ++ x)
        for (int y = 0; y < 3; ++y)
            dungeon.at(x, y, 0) = Tile::EMPTY;

    EXPECT_EQ(nextStep(dungeon, { 0, 2, 0 }, { 2, 0, 0 }), (sf::Vector3i{1, -1, 0}));
}

TEST(pathfinding, nextStepTurn) {
    Dungeon dungeon;
    dungeon.resize(1);
    dungeon[0].generateBlank({ 3, 3 });
    dungeon.at(0, 0, 0) = Tile::WALL;
    dungeon.at(1, 0, 0) = Tile::EMPTY;
    dungeon.at(2, 0, 0) = Tile::EMPTY;
    dungeon.at(0, 1, 0) = Tile::WALL;
    dungeon.at(1, 1, 0) = Tile::WALL;
    dungeon.at(2, 1, 0) = Tile::EMPTY;
    dungeon.at(0, 2, 0) = Tile::EMPTY;
    dungeon.at(1, 2, 0) = Tile::EMPTY;
    dungeon.at(2, 2, 0) = Tile::EMPTY;

    EXPECT_EQ(nextStep(dungeon, { 0, 2, 0 }, { 1, 0, 0 }), (sf::Vector3i{ 1,  0, 0 }));
}
