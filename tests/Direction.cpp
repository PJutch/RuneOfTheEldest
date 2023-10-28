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

#include "util/Direction.hpp"

#include <gtest/gtest.h>

TEST(Direction, turn90LeftUniform) {
    EXPECT_EQ(util::turn90Left(sf::Vector2i{0, -1}), (sf::Vector2i{-1, 0}));
}

TEST(Direction, turn90Left) {
    EXPECT_EQ(util::turn90Left(sf::Vector2i{3, 4}), (sf::Vector2i{4, -3}));
}

TEST(Direction, turn90RightUniform) {
    EXPECT_EQ(util::turn90Right(sf::Vector2i{0, -1}), (sf::Vector2i{1, 0}));
}

TEST(Direction, turn90Right) {
    EXPECT_EQ(util::turn90Right(sf::Vector2i{3, 4}), (sf::Vector2i{-4, 3}));
}

TEST(Direction, turnDirection45Left) {
    EXPECT_EQ(util::turnDirection45Left(sf::Vector2i{0, -1}), (sf::Vector2i{-1, -1}));
}

TEST(Direction, nonzeroIsAllSubset) {
    for (auto dir : util::nonzeroDirections<int>)
        EXPECT_NE(std::ranges::find(util::directions<int>, dir), util::directions<int>.end());
}
