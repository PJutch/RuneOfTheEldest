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

#include "RoomGenerator/BasicRoomGenerator.hpp"

#include "Dungeon.hpp"

#include "geometry.hpp"

#include <gtest/gtest.h>

bool isValid(sf::IntRect rect) noexcept {
    return rect.width > 0 && rect.height > 0;
}

testing::AssertionResult hasSingleRoom(const Dungeon& dungeon, int z, sf::IntRect area) {
    sf::IntRect room = shrinkBottomRight(area, {1, 1});
    
    for (int x = 0; x < dungeon.shape().x; ++ x)
        for (int y = 0; y < dungeon.shape().y; ++ y)
            if (room.contains(x, y)) {
                if (dungeon.at(x, y, z) != Tile::EMPTY)
                    return testing::AssertionFailure()
                         << "tile at " << x << ", " << y 
                         << " should be empty but it is not";
            } else
                if (dungeon.at(x, y, z) != Tile::WALL)
                    return testing::AssertionFailure()
                         << "tile at " << x << ", " << y 
                         << " should be unseen but it is not";
    
    return testing::AssertionSuccess();
}

TEST(BasicRoomGenerator, generateRoom) {
    Dungeon dungeon;
    dungeon.assign({ 10, 7, 1 });

    sf::IntRect room{3, 2, 6, 5};
    BasicRoomGenerator generator;
    generator.dungeon(dungeon);
    generator(0, Area{ room });

    EXPECT_TRUE(hasSingleRoom(dungeon, 0, room));
}

TEST(BasicRoomGenerator, generateRoomNearEdge) {
    Dungeon dungeon;
    dungeon.assign({ 8, 7, 1 });

    sf::IntRect room{1, 2, 7, 5};
    BasicRoomGenerator generator;
    generator.dungeon(dungeon);
    generator(0, Area{ room });

    EXPECT_TRUE(hasSingleRoom(dungeon, 0, room));
}

TEST(BasicRoomGenerator, generateRoom3x3) {
    Dungeon dungeon;
    dungeon.assign({ 7, 7, 1 });

    sf::IntRect room{3, 2, 3, 3};
    BasicRoomGenerator generator;
    generator.dungeon(dungeon);
    generator(0, Area{ room });

    EXPECT_TRUE(hasSingleRoom(dungeon, 0, room));
}

TEST(BasicRoomGenerator, generateRoom2x2) {
    Dungeon dungeon;
    dungeon.assign({ 7, 7, 1 });

    sf::IntRect room{3, 2, 2, 2};
    BasicRoomGenerator generator;
    generator.dungeon(dungeon);
    generator(0, Area{ room });

    EXPECT_TRUE(hasSingleRoom(dungeon, 0, room));
}

TEST(BasicRoomGenerator, generateRoom1x1) {
    Dungeon dungeon;
    dungeon.assign({ 7, 7, 1 });

    sf::IntRect room{3, 2, 1, 1};
    BasicRoomGenerator generator;
    generator.dungeon(dungeon);
    generator(0, Area{ room });

    EXPECT_TRUE(hasSingleRoom(dungeon, 0, room));
}
