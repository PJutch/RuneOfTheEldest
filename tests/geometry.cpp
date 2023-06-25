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

#include "util/geometry.hpp"

#include <gtest/gtest.h>

TEST(geometry, addX) {
    sf::Vector2i vec{5, 10};
    int dx = 14;
    sf::Vector2i result{19, 10};
    EXPECT_EQ(util::addX(vec, dx), result);
}

TEST(geometry, subX) {
    sf::Vector2i vec{5, 10};
    int dx = 14;
    sf::Vector2i result{-9, 10};
    EXPECT_EQ(util::subX(vec, dx), result);
}

TEST(geometry, addY) {
    sf::Vector2i vec{5, 10};
    int dy = 14;
    sf::Vector2i result{5, 24};
    EXPECT_EQ(util::addY(vec, dy), result);
}

TEST(geometry, subY) {
    sf::Vector2i vec{5, 10};
    int dy = 14;
    sf::Vector2i result{5, -4};
    EXPECT_EQ(util::subY(vec, dy), result);
}

TEST(geometry, extendTopLeft) {
    sf::IntRect rect{7, 4, 3, 2};
    sf::Vector2i extension{4, 2};
    sf::IntRect result{3, 2, 7, 4};
    EXPECT_EQ(util::extendTopLeft(rect, extension), result);
}

TEST(geometry, extendBottomRight) {
    sf::IntRect rect{7, 4, 3, 2};
    sf::Vector2i extension{4, 2};
    sf::IntRect result{7, 4, 7, 4};
    EXPECT_EQ(util::extendBottomRight(rect, extension), result);
}

TEST(geometry, shrinkTopLeft) {
    sf::IntRect result{7, 4, 3, 2};
    sf::IntRect rect{3, 2, 7, 4};
    sf::Vector2i shrink{4, 2};
    EXPECT_EQ(util::shrinkTopLeft(rect, shrink), result);
}

TEST(geometry, shrinkBottomRight) {
    sf::IntRect rect{7, 4, 7, 4};
    sf::IntRect result{7, 4, 3, 2};
    sf::Vector2i shrink{4, 2};
    EXPECT_EQ(util::shrinkBottomRight(rect, shrink), result);
}

TEST(geometry, uniformNorm) {
    EXPECT_EQ(util::uniformNorm(sf::Vector3i{ 3, 5, 4 }), 5);
}

TEST(geometry, uniformNormZero) {
    EXPECT_EQ(util::uniformNorm(sf::Vector3i{ 0, 0, 0 }), 0);
}

TEST(geometry, uniformNormNegative) {
    EXPECT_EQ(util::uniformNorm(sf::Vector3i{ 3, -5, 4 }), 5);
}

TEST(geometry, uniformDistance) {
    EXPECT_EQ(util::uniformDistance(sf::Vector3i{1, 2, 3}, sf::Vector3i{5, 7, 10}), 7);
}

TEST(geometry, uniformDistanceZero) {
    EXPECT_EQ(util::uniformDistance(sf::Vector3i{ 1, 2, 3 }, sf::Vector3i{ 1, 2, 3 }), 0);
}

TEST(geometry, uniformDistanceNegative) {
    EXPECT_EQ(util::uniformDistance(sf::Vector3i{ 5, 7, 10 }, sf::Vector3i{ 1, 2, 3 }), 7);
}

TEST(geometry, uniformNorm2D) {
    EXPECT_EQ(util::uniformNorm(sf::Vector2i{ 3, 5 }), 5);
}

TEST(geometry, uniformDistance2D) {
    EXPECT_EQ(util::uniformDistance(sf::Vector2i{ 1, 2}, sf::Vector2i{ 5, 7 }), 5);
}

TEST(geometry, turn90LeftUniform) {
    EXPECT_EQ(util::turn90Left(sf::Vector2i{ 0, -1 }), (sf::Vector2i{ -1, 0 }));
}

TEST(geometry, turn90Left) {
    EXPECT_EQ(util::turn90Left(sf::Vector2i{ 3, 4 }), (sf::Vector2i{ 4, -3 }));
}

TEST(geometry, turn90RightUniform) {
    EXPECT_EQ(util::turn90Right(sf::Vector2i{ 0, -1 }), (sf::Vector2i{ 1, 0 }));
}

TEST(geometry, turn90Right) {
    EXPECT_EQ(util::turn90Right(sf::Vector2i{ 3, 4 }), (sf::Vector2i{ -4, 3 }));
}

TEST(geometry, turnDirection45Left) {
    EXPECT_EQ(util::turnDirection45Left(sf::Vector2i{ 0, -1 }), (sf::Vector2i{ -1, -1 }));
}

TEST(geometry, norm) {
    EXPECT_EQ(util::norm(sf::Vector2i{3, 4}), 5);
}

TEST(geometry, normZero) {
    EXPECT_EQ(util::norm(sf::Vector2i{ 0, 0 }), 0);
}

TEST(geometry, normNegativeComponents) {
    EXPECT_EQ(util::norm(sf::Vector2i{ -3, -4 }), 5);
}

TEST(geometry, normIrrational) {
    EXPECT_EQ(util::norm(sf::Vector2i{ 1, 1 }), std::sqrt(2));
}

TEST(geometry, distance) {
    EXPECT_EQ(util::distance(sf::Vector2i{ 1, 4 }, sf::Vector2i{ 4, 8 }), 5);
}

TEST(geometry, distanceZero) {
    EXPECT_EQ(util::distance(sf::Vector2i{ 1, 4 }, sf::Vector2i{ 1, 4 }), 0);
}

TEST(geometry, distanceNegativeComponents) {
    EXPECT_EQ(util::distance(sf::Vector2i{ 4, 8 }, sf::Vector2i{ 1, 4 }), 5);
}

TEST(geometry, distanceIrrational) {
    EXPECT_EQ(util::distance(sf::Vector2i{ 2, 2 }, sf::Vector2i{ 3, 3 }), std::sqrt(2));
}
