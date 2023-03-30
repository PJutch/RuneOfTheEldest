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

TEST(LevelTest, generateRoom) {
    sf::Vector2i shape{10, 7};
    sf::IntRect room{2, 1, 6, 5};
    sf::IntRect roomFloor{3, 2, 4, 3};

    Level level;
    level.generate(shape, room);

    for (int x = 0; x < shape.x; ++ x)
        for (int y = 0; y < shape.y; ++ y)
            if (roomFloor.contains(x, y))
                EXPECT_EQ(level.at(x, y), Level::Tile::EMPTY);
            else if (room.contains(x, y))
                EXPECT_EQ(level.at(x, y), Level::Tile::WALL);
            else
                EXPECT_EQ(level.at(x, y), Level::Tile::UNSEEN);
}

TEST(LevelTest, generateRoomNearEdge) {
    sf::Vector2i shape{8, 7};
    sf::IntRect room{0, 1, 7, 5};
    sf::IntRect roomFloor{1, 2, 5, 3};

    Level level;
    level.generate(shape, room);

    for (int x = 0; x < shape.x; ++ x)
        for (int y = 0; y < shape.y; ++ y)
            if (roomFloor.contains(x, y))
                EXPECT_EQ(level.at(x, y), Level::Tile::EMPTY);
            else if (room.contains(x, y))
                EXPECT_EQ(level.at(x, y), Level::Tile::WALL);
            else
                EXPECT_EQ(level.at(x, y), Level::Tile::UNSEEN);
}

TEST(LevelTest, generateRoom3x3) {
    sf::Vector2i shape{7, 7};
    sf::IntRect room{2, 1, 3, 3};
    sf::IntRect roomFloor{3, 2, 1, 1};

    Level level;
    level.generate(shape, room);

    for (int x = 0; x < shape.x; ++ x)
        for (int y = 0; y < shape.y; ++ y)
            if (roomFloor.contains(x, y))
                EXPECT_EQ(level.at(x, y), Level::Tile::EMPTY);
            else if (room.contains(x, y))
                EXPECT_EQ(level.at(x, y), Level::Tile::WALL);
            else
                EXPECT_EQ(level.at(x, y), Level::Tile::UNSEEN);
}

TEST(LevelTest, generateRoom2x2) {
    sf::Vector2i shape{7, 7};
    sf::IntRect room{2, 1, 2, 2};

    Level level;
    level.generate(shape, room);

    for (int x = 0; x < shape.x; ++ x)
        for (int y = 0; y < shape.y; ++ y)
            if (room.contains(x, y))
                EXPECT_EQ(level.at(x, y), Level::Tile::WALL);
            else
                EXPECT_EQ(level.at(x, y), Level::Tile::UNSEEN);
}

TEST(LevelTest, generateRoom1x1) {
    sf::Vector2i shape{7, 7};
    sf::IntRect room{2, 1, 1, 1};

    Level level;
    level.generate(shape, room);

    for (int x = 0; x < shape.x; ++ x)
        for (int y = 0; y < shape.y; ++ y)
            if (room.contains(x, y))
                EXPECT_EQ(level.at(x, y), Level::Tile::WALL);
            else
                EXPECT_EQ(level.at(x, y), Level::Tile::UNSEEN);
}
