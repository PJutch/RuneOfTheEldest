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

#include "util/parse.hpp"

#include "util/assert.hpp"

#include <gtest/gtest.h>

#include <sstream>

TEST(parse, isSpace) {
	EXPECT_TRUE(util::isSpace(' '));
	EXPECT_TRUE(util::isSpace('\n'));
	EXPECT_FALSE(util::isSpace('c'));
	EXPECT_FALSE(util::isSpace('5'));
	EXPECT_FALSE(util::isSpace(','));
}

TEST(parse, isDigit) {
	EXPECT_FALSE(util::isDigit(' '));
	EXPECT_FALSE(util::isDigit('\n'));
	EXPECT_FALSE(util::isDigit('c'));
	EXPECT_TRUE(util::isDigit('5'));
	EXPECT_FALSE(util::isDigit(','));
}

TEST(parse, strip) {
	using namespace std::literals;
	EXPECT_EQ(util::strip(" \nabc "), "abc"sv);
	EXPECT_EQ(util::strip("abc"), "abc"sv);
	EXPECT_EQ(util::strip("  "), ""sv);
}

TEST(parse, parseUint) {
	EXPECT_EQ(util::parseUint<int>("34"), 34);
	EXPECT_THROW(util::parseUint<int>(""), util::EmptyStringError);
	EXPECT_THROW(util::parseUint<int>("f"), util::NotADigitError);
}

TEST(parse, parseInt) {
	EXPECT_EQ(util::parseInt<int>("34"), 34);
	EXPECT_EQ(util::parseInt<int>("+34"), 34);
	EXPECT_EQ(util::parseInt<int>("-15"), -15);
	EXPECT_THROW(util::parseInt<int>(""), util::EmptyStringError);
	EXPECT_THROW(util::parseInt<int>("f"), util::NotADigitError);
}

TEST(parse, parseReal) {
	EXPECT_EQ(util::parseReal<double>("34"), 34);
	EXPECT_EQ(util::parseReal<double>("+34"), 34);
	EXPECT_EQ(util::parseReal<double>("-15"), -15);
	EXPECT_EQ(util::parseReal<double>("12.7"), 12.7);
	EXPECT_EQ(util::parseReal<double>("+12.7"), 12.7);
	EXPECT_EQ(util::parseReal<double>("-12.7"), -12.7);
	EXPECT_EQ(util::parseReal<double>(".5"), 0.5);
	EXPECT_EQ(util::parseReal<double>("0.01"), 0.01);
	EXPECT_EQ(util::parseReal<double>("1e3"), 1000);
	EXPECT_EQ(util::parseReal<double>("1e-3"), 0.001);
	EXPECT_EQ(util::parseReal<double>("1E3"), 1000);
	EXPECT_EQ(util::parseReal<double>("1.2e1"), 12);
	EXPECT_THROW(util::parseReal<double>(""), util::EmptyStringError);
	EXPECT_THROW(util::parseReal<double>("f"), util::NotADigitError);
	EXPECT_THROW(util::parseReal<double>("1.+"), util::NotADigitError);
	EXPECT_THROW(util::parseReal<double>("1e"), util::EmptyStringError);
}

TEST(parse, forEachStrippedLine) {
	std::istringstream stream{"a\n  b \n c\n"};
	util::forEachStrippedLine(stream, [](std::string_view line, int lineIndex) {
		using namespace std::literals;
		switch (lineIndex) {
		case 0: TROTE_ASSERT(line == "a"sv); break;
		case 1: TROTE_ASSERT(line == "b"sv); break;
		case 2: TROTE_ASSERT(line == "c"sv); break;
		default: TROTE_ASSERT(false, "there shouldn't be more lines"); break;
		}
	});
}

TEST(parse, forEachStrippedLineEmpty) {
	std::istringstream stream{ "" };
	util::forEachStrippedLine(stream, [](std::string_view line, int lineIndex) {
		TROTE_ASSERT(false, "there shouldn't be more lines");
	});
}
