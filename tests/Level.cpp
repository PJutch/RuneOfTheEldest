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

bool isValid(sf::IntRect rect) noexcept {
    return rect.width > 0 && rect.height > 0;
}

testing::AssertionResult hasSingleRoom(Level& level, sf::IntRect room) {
    sf::IntRect roomFloor{room.left + 1, room.top + 1, 
                          room.width - 2, room.height - 2};
    
    for (int x = 0; x < level.shape().x; ++ x)
        for (int y = 0; y < level.shape().y; ++ y)
            if (isValid(roomFloor) && roomFloor.contains(x, y)) {
                if (level.at(x, y) != Level::Tile::EMPTY)
                    return testing::AssertionFailure()
                         << "tile at " << x << ", " << y 
                         << "should be empty but it is not";
            } else if (room.contains(x, y)) {
                if (level.at(x, y) != Level::Tile::WALL)
                    return testing::AssertionFailure()
                         << "tile at " << x << ", " << y 
                         << "should be wall but it is not";
            } else
                if (level.at(x, y) != Level::Tile::UNSEEN)
                    return testing::AssertionFailure()
                         << "tile at " << x << ", " << y 
                         << "should be unseen but it is not";
    
    return testing::AssertionSuccess();
}

TEST(LevelTest, generateBlankShape) {
    sf::Vector2i shape{10, 7};

    RandomEngine randomEngine;
    Level level{randomEngine};
    level.generateBlank(shape);

    EXPECT_EQ(level.shape(), shape);
}

TEST(LevelTest, generateBlank) {
    RandomEngine randomEngine;
    Level level{randomEngine};
    level.generateBlank({10, 7});

    for (int x = 0; x < level.shape().x; ++ x)
        for (int y = 0; y < level.shape().y; ++ y)
            EXPECT_EQ(level.at(x, y), Level::Tile::UNSEEN);
}

TEST(LevelTest, generateRoom) {
    RandomEngine randomEngine;
    Level level{randomEngine};
    level.generateBlank({10, 7});

    sf::IntRect room{2, 1, 6, 5};
    level.generateRoom(room);

    EXPECT_TRUE(hasSingleRoom(level, room));
}

TEST(LevelTest, generateRoomNearEdge) {
    RandomEngine randomEngine;
    Level level{randomEngine};
    level.generateBlank({8, 7});

    sf::IntRect room{0, 1, 7, 5};
    level.generateRoom(room);

    EXPECT_TRUE(hasSingleRoom(level, room));
}

TEST(LevelTest, generateRoom3x3) {
    RandomEngine randomEngine;
    Level level{randomEngine};
    level.generateBlank({7, 7});

    sf::IntRect room{2, 1, 3, 3};
    level.generateRoom(room);

    EXPECT_TRUE(hasSingleRoom(level, room));
}

TEST(LevelTest, generateRoom2x2) {
    RandomEngine randomEngine;
    Level level{randomEngine};
    level.generateBlank({7, 7});

    sf::IntRect room{2, 1, 2, 2};
    level.generateRoom(room);

    EXPECT_TRUE(hasSingleRoom(level, room));
}

TEST(LevelTest, generateRoom1x1) {
    RandomEngine randomEngine;
    Level level{randomEngine};
    level.generateBlank({7, 7});

    sf::IntRect room{2, 1, 1, 1};
    level.generateRoom(room);

    EXPECT_TRUE(hasSingleRoom(level, room));
}
