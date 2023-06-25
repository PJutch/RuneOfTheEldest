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

#include "generation/BasicRoomGenerator.hpp"

#include "util/geometry.hpp"

#include <gtest/gtest.h>

bool isValid(sf::IntRect rect) noexcept {
    return rect.width > 0 && rect.height > 0;
}

testing::AssertionResult hasSingleRoom(const core::World& world, int z, sf::IntRect area) {
    sf::IntRect room = util::shrinkBottomRight(area, {1, 1});
    
    for (int x = 0; x < world.tiles().shape().x; ++x)
        for (int y = 0; y < world.tiles().shape().y; ++ y)
            if (room.contains(x, y)) {
                if (world.tiles()[{x, y, z}] != Tile::EMPTY)
                    return testing::AssertionFailure()
                         << "tile at " << x << ", " << y 
                         << " should be empty but it is not";
            } else
                if (world.tiles()[{x, y, z}] != Tile::WALL)
                    return testing::AssertionFailure()
                         << "tile at " << x << ", " << y 
                         << " should be wall but it is not";
    
    return testing::AssertionSuccess();
}

TEST(BasicRoomGenerator, generateRoom) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 10, 7, 1 }, Tile::WALL);

    sf::IntRect room{3, 2, 6, 5};
    generation::BasicRoomGenerator generator{ world };
    generator(0, generation::Area{ room });

    EXPECT_TRUE(hasSingleRoom(*world, 0, room));
}

TEST(BasicRoomGenerator, generateRoomNearEdge) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 8, 7, 1 }, Tile::WALL);

    sf::IntRect room{1, 2, 7, 5};
    generation::BasicRoomGenerator generator{ world };
    generator(0, generation::Area{ room });

    EXPECT_TRUE(hasSingleRoom(*world, 0, room));
}

TEST(BasicRoomGenerator, generateRoom3x3) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 7, 7, 1 }, Tile::WALL);

    sf::IntRect room{3, 2, 3, 3};
    generation::BasicRoomGenerator generator{ world };
    generator(0, generation::Area{ room });

    EXPECT_TRUE(hasSingleRoom(*world, 0, room));
}

TEST(BasicRoomGenerator, generateRoom2x2) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 7, 7, 1 }, Tile::WALL);

    sf::IntRect room{3, 2, 2, 2};
    generation::BasicRoomGenerator generator{ world };
    generator(0, generation::Area{ room });

    EXPECT_TRUE(hasSingleRoom(*world, 0, room));
}

TEST(BasicRoomGenerator, generateRoom1x1) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 7, 7, 1 }, Tile::WALL);

    sf::IntRect room{3, 2, 1, 1};
    generation::BasicRoomGenerator generator{world};
    generator(0, generation::Area{ room });

    EXPECT_TRUE(hasSingleRoom(*world, 0, room));
}
