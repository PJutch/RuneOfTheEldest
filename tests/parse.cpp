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
	EXPECT_EQ(util::parseUint("34"), 34);
	EXPECT_THROW(util::parseUint(""), util::EmptyStringError);
	EXPECT_THROW(util::parseUint("f"), util::NotADigitError);
}

TEST(parse, parseInt) {
	EXPECT_EQ(util::parseInt("34"), 34);
	EXPECT_EQ(util::parseInt("+34"), 34);
	EXPECT_EQ(util::parseInt("-15"), -15);
	EXPECT_THROW(util::parseInt(""), util::EmptyStringError);
	EXPECT_THROW(util::parseInt("f"), util::NotADigitError);
}

TEST(parse, parseReal) {
	EXPECT_EQ(util::parseReal("34"), 34);
	EXPECT_EQ(util::parseReal("+34"), 34);
	EXPECT_EQ(util::parseReal("-15"), -15);
	EXPECT_EQ(util::parseReal("12.7"), 12.7);
	EXPECT_EQ(util::parseReal("+12.7"), 12.7);
	EXPECT_EQ(util::parseReal("-12.7"), -12.7);
	EXPECT_EQ(util::parseReal(".5"), 0.5);
	EXPECT_EQ(util::parseReal("0.01"), 0.01);
	EXPECT_EQ(util::parseReal("1e3"), 1000);
	EXPECT_EQ(util::parseReal("1e-3"), 0.001);
	EXPECT_EQ(util::parseReal("1E3"), 1000);
	EXPECT_EQ(util::parseReal("1.2e1"), 12);
	EXPECT_THROW(util::parseReal(""), util::EmptyStringError);
	EXPECT_THROW(util::parseReal("f"), util::NotADigitError);
	EXPECT_THROW(util::parseReal("1.+"), util::NotADigitError);
	EXPECT_THROW(util::parseReal("1e"), util::EmptyStringError);
}

TEST(parse, parseBool) {
	EXPECT_TRUE(util::parseBool("true"));
	EXPECT_FALSE(util::parseBool("false"));
	EXPECT_THROW(util::parseBool(""), util::EmptyStringError);
	EXPECT_THROW(util::parseBool("garbage"), util::NotABoolError);
	EXPECT_THROW(util::parseBool("False"), util::NotABoolError);
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
	std::istringstream stream{ "a\n\n  " };
	util::forEachStrippedLine(stream, [](std::string_view line, int lineIndex) {
		using namespace std::literals;
		switch (lineIndex) {
		case 0: TROTE_ASSERT(line == "a"sv); break;
		default: TROTE_ASSERT(false, "there shouldn't be more lines"); break;
		}
	});
}

TEST(parse, forEachStrippedLineComments) {
	std::istringstream stream{ "a # b\n # cde " };
	util::forEachStrippedLine(stream, [](std::string_view line, int lineIndex) {
		using namespace std::literals;
		switch (lineIndex) {
		case 0: TROTE_ASSERT(line == "a"sv); break;
		default: TROTE_ASSERT(false, "there shouldn't be more lines"); break;
		}
	});
}

TEST(parse, forEachStrippedLineNone) {
	std::istringstream stream{ "" };
	util::forEachStrippedLine(stream, [](std::string_view line, int lineIndex) {
		TROTE_ASSERT(false, "there shouldn't be more lines");
	});
}

TEST(parse, parseKeyValuePair) {
	using namespace std::literals;
	EXPECT_EQ(util::parseKeyValuePair("ab cde"), (std::pair{"ab"sv, "cde"sv}));
	EXPECT_EQ(util::parseKeyValuePair("ab  cde"), (std::pair{ "ab"sv, "cde"sv }));
	EXPECT_EQ(util::parseKeyValuePair("ab cde fg"), (std::pair{ "ab"sv, "cde fg"sv }));
	EXPECT_THROW(util::parseKeyValuePair(""), util::EmptyStringError);
	EXPECT_THROW(util::parseKeyValuePair("ab "), util::NoValueError);
}

TEST(parse, parseMapping) {
	std::istringstream stream{ "ab  cde \n fg hi" };
	using namespace std::literals;
	EXPECT_EQ(util::parseMapping(stream), (std::unordered_map{ std::pair{"ab"s, "cde"s}, std::pair{"fg"s, "hi"s} }));
}

TEST(parse, parseMappingEmpty) {
	std::istringstream stream{ "" };
	EXPECT_TRUE(util::parseMapping(stream).empty());
}

TEST(parse, parseMappingComments) {
	std::istringstream stream{ "ab  cde  # ??\n# bla-bla \n fg hi" };
	using namespace std::literals;
	EXPECT_EQ(util::parseMapping(stream), (std::unordered_map{ std::pair{"ab"s, "cde"s}, std::pair{"fg"s, "hi"s} }));
}

TEST(parse, parseList) {
	using namespace std::literals;
	EXPECT_EQ(util::parseList("ab, cde"), (std::vector{"ab"sv, " cde"sv}));
	EXPECT_EQ(util::parseList("ab,cde"), (std::vector{"ab"sv, "cde"sv}));
	EXPECT_EQ(util::parseList("ab,"), (std::vector{"ab"sv}));
	EXPECT_EQ(util::parseList("abc"), (std::vector{"abc"sv}));
	EXPECT_EQ(util::parseList(""), (std::vector<std::string_view>{}));
	EXPECT_EQ(util::parseList("a, b, c, d"), (std::vector{"a"sv, " b"sv, " c"sv, " d"sv}));
	EXPECT_EQ(util::parseList(",,"), (std::vector{""sv, ""sv}));
}

