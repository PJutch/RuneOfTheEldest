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

#include "RoomGenerator.hpp"

#include "geometry.hpp"

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
                         << " should be empty but it is not";
            } else if (room.contains(x, y)) {
                if (level.at(x, y) != Level::Tile::WALL)
                    return testing::AssertionFailure()
                         << "tile at " << x << ", " << y 
                         << " should be wall but it is not";
            } else
                if (level.at(x, y) != Level::Tile::UNSEEN)
                    return testing::AssertionFailure()
                         << "tile at " << x << ", " << y 
                         << " should be unseen but it is not";
    
    return testing::AssertionSuccess();
}

TEST(BasicRoomGenerator, generateRoom) {
    auto level = std::make_shared<Level>();
    level->generateBlank({10, 7});

    sf::IntRect room{3, 2, 6, 5};
    BasicRoomGenerator{level}(room);

    EXPECT_TRUE(hasSingleRoom(*level, extendTopLeft(room, {1, 1})));
}

TEST(BasicRoomGenerator, generateRoomNearEdge) {
    auto level = std::make_shared<Level>();
    level->generateBlank({8, 7});

    sf::IntRect room{1, 2, 7, 5};
    BasicRoomGenerator{level}(room);

    EXPECT_TRUE(hasSingleRoom(*level, extendTopLeft(room, {1, 1})));
}

TEST(BasicRoomGenerator, generateRoom3x3) {
    auto level = std::make_shared<Level>();
    level->generateBlank({7, 7});

    sf::IntRect room{3, 2, 3, 3};
    BasicRoomGenerator{level}(room);

    EXPECT_TRUE(hasSingleRoom(*level, extendTopLeft(room, {1, 1})));
}

TEST(BasicRoomGenerator, generateRoom2x2) {
    auto level = std::make_shared<Level>();
    level->generateBlank({7, 7});

    sf::IntRect room{3, 2, 2, 2};
    BasicRoomGenerator{level}(room);

    EXPECT_TRUE(hasSingleRoom(*level, extendTopLeft(room, {1, 1})));
}

TEST(BasicRoomGenerator, generateRoom1x1) {
    auto level = std::make_shared<Level>();
    level->generateBlank({7, 7});

    sf::IntRect room{3, 2, 1, 1};
    BasicRoomGenerator{level}(room);

    EXPECT_TRUE(hasSingleRoom(*level, extendTopLeft(room, {1, 1})));
}
