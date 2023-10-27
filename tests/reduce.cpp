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

#include "util/reduce.hpp"

#include <gtest/gtest.h>

#include <vector>

TEST(reduce, reduce) {
    std::vector<int> v{1, 2, 3, 4, 5};
    EXPECT_EQ(util::reduce(v), 15);
}

TEST(reduce, init) {
    std::vector<int> v{1, 2, 3, 4, 5};
    EXPECT_EQ(util::reduce(v, 1), 16);
}

TEST(reduce, conversion) {
    std::vector<double> v{1, 2, 3, 4, 5};
    EXPECT_EQ(util::reduce(v, 0.5), 15.5);
}

TEST(reduce, op) {
    std::vector<int> v{1, 2, 3, 4, 5};
    EXPECT_EQ(util::reduce(v, 1, std::multiplies<>{}), 120);
}

TEST(reduce, proj) {
    std::vector<std::pair<int, int>> v{{2, 1}, {4, 2}, {5, 3}, {1, 4}, {7, 5}};
    EXPECT_EQ(util::reduce(v, 0, std::plus<>{}, &std::pair<int, int>::second), 15);
}
