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

#include "util/Map.hpp"

#include <gtest/gtest.h>

TEST(Map, getPtr) {
    util::UnorderedMap<int, int> map{
        {0, 1},
        {2, 5},
        {3, 7}
    };

    EXPECT_EQ(util::getPtr(map, 2), &map[2]);
}

TEST(Map, getPtrNull) {
    util::UnorderedMap<int, int> map{
        {0, 1},
        {2, 5},
        {3, 7}
    };

    EXPECT_EQ(util::getPtr(map, 1), nullptr);
}

TEST(Map, getPtrConst) {
    const util::UnorderedMap<int, int> map{
        {0, 1},
        {2, 5},
        {3, 7}
    };

    EXPECT_EQ(util::getPtr(map, 2), &map.at(2));
}

TEST(Map, getPtrConstNull) {
    const util::UnorderedMap<int, int> map{
        {0, 1},
        {2, 5},
        {3, 7}
    };

    EXPECT_EQ(util::getPtr(map, 1), nullptr);
}

TEST(Map, getOptional) {
    util::UnorderedMap<int, int> map{
        {0, 1},
        {2, 5},
        {3, 7}
    };

    EXPECT_EQ(*util::getOptional(map, 2), 5);
}

TEST(Map, getOptionalNull) {
    util::UnorderedMap<int, int> map{
        {0, 1},
        {2, 5},
        {3, 7}
    };

    EXPECT_FALSE(util::getOptional(map, 1));
}
