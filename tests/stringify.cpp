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

#include "util/stringify.hpp"

#include <gtest/gtest.h>


TEST(stringify, stringifyCharMap) {
	util::Array3D<char> data;
	data.assign({2, 2, 2}, 'x');

	data[{0, 0, 0}] = 'a';
	data[{1, 0, 0}] = 'b';
	data[{0, 1, 0}] = 'c';
	data[{1, 1, 0}] = 'd';
	data[{0, 0, 1}] = 'e';
	data[{1, 0, 1}] = 'f';
	data[{0, 1, 1}] = 'g';
	data[{1, 1, 1}] = 'h';

	EXPECT_EQ(util::stringifyCharMap(data), "ab\ncd\n--\nef\ngh\n");
}

TEST(stringify, stringifyCharMapComplexShape) {
	util::Array3D<char> data;
	data.assign({3, 4, 2}, 'x');

	EXPECT_EQ(util::stringifyCharMap(data), "xxx\nxxx\nxxx\nxxx\n---\nxxx\nxxx\nxxx\nxxx\n");
}
