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

#ifndef PARSE_HPP_
#define PARSE_HPP_

#include "Exception.hpp"
#include "Map.hpp"
#include "Array3D.hpp"

#include <format>
#include <iostream>
#include <algorithm>
#include <string>
#include <string_view>
#include <concepts>
#include <cctype>

namespace util {
	/// More safe wrapper around isspace
	inline bool isSpace(char c) noexcept {
		return std::isspace(static_cast<unsigned char>(c));
	}

	/// More safe wrapper around isdigit
	inline bool isDigit(char c) noexcept {
		return std::isdigit(static_cast<unsigned char>(c));
	}

	/// @brief Strip spaces from left and right
	/// @details Removes all chars satisfying isSpace from left and right.
	/// Returns subview of input view
	inline std::string_view strip(std::string_view s) noexcept {
		auto begin = std::ranges::find_if_not(s, &isSpace);
		if (begin == s.end())
			return {};

		auto end = std::ranges::find_if_not(s.rbegin(), s.rend(), &isSpace).base();
		return { begin, end };
	}

	/// @brief Strip spaces from right
	/// @details Removes all chars satisfying isSpace from right.
	/// Returns subview of input view
	inline std::string_view rightStrip(std::string_view s) noexcept {
		auto end = std::ranges::find_if_not(s.rbegin(), s.rend(), &isSpace).base();
		return {s.begin(), end};
	}

	/// @brief Strip comment from line
	/// @details Removes all chars after '#'.
	/// Returns subview of input view
	inline std::string_view stripComment(std::string_view s) noexcept {
		auto commentBegin = std::ranges::find(s, '#');
		return { s.begin(), commentBegin };
	}

	/// Error while parsing smth
	class ParseError : public RuntimeError {
	public:
		ParseError(const std::string& message, std::string_view parsed, Stacktrace currentStacktrace = {}) noexcept :
			RuntimeError{ std::format("Error while parsing \"{}\": {}", parsed, message), std::move(currentStacktrace)} {}
	};

	/// Unexpected empty string
	class EmptyStringError : public ParseError {
	public:
		EmptyStringError(std::string_view parsed, Stacktrace currentStacktrace = {}) noexcept :
			ParseError{ "Unexpected empty string", parsed, std::move(currentStacktrace) } {}
	};

	/// Char requiered to be digit
	class NotADigitError : public ParseError {
	public:
		NotADigitError(char c, std::string_view parsed, Stacktrace currentStacktrace = {}) noexcept :
			ParseError{ std::format("'{}' is not a digit", c), parsed, std::move(currentStacktrace)} {}
	};

	/// @brief Parses s as a nonnegative integer without sign
	/// @tparam ResultT Type of the result. Not nesessary unsigned or integer.
	/// @warning Expects that all chars in s represents the number (no trailing spaices and etc.)
	/// @throws EmptyStringError when s is empty
	/// @throws NotADigitError if encountered nondigit char
	template <typename ResultT = int>
	ResultT parseUint(std::string_view s) {
		if (s.empty())
			throw EmptyStringError{s};

		ResultT result{};
		for (char c : s) {
			if (!isDigit(c))
				throw NotADigitError{ c, s };

			result *= 10;
			result += (c - '0');
		}

		return result;
	}

	/// @brief Parses s as a integer with optional sign
	/// @tparam ResultT Type of the result. Not nesessary integer.
	/// @warning Expects that all chars in s represents the number (no trailing spaices and etc.)
	/// @throws EmptyStringError when s is empty
	/// @throws NotADigitError if encountered wrong char
	template <typename ResultT = int>
	ResultT parseInt(std::string_view s) {
		if (s.empty())
			throw EmptyStringError{s};

		if (s.front() == '-') {
			s.remove_prefix(1);
			return -parseUint<ResultT>(s);
		}

		if (s.front() == '+')
			s.remove_prefix(1);
		return parseUint<ResultT>(s);
	}

	/// @brief Parses s as a real number in format [+-]?\d*.?\d*(e\d+)?
	/// @warning Expects that all chars in s represents the number (no trailing spaices and etc.)
	/// @throws EmptyStringError when s is empty
	/// @throws NotADigitError if encountered wrong char
	template <typename ResultT = double>
	ResultT parseReal(std::string_view s) {
		if (s.empty())
			throw EmptyStringError{ s };

		if (s.front() == '-') {
			s.remove_prefix(1);
			return -parseReal<ResultT>(s);
		}

		if (s.front() == '+')
			s.remove_prefix(1);

		using namespace std::literals;

		auto nextPart = std::ranges::find_first_of(s, ".eE"sv);
		ResultT result = (nextPart != s.begin()) ? parseUint<ResultT>({ s.begin(), nextPart }) : ResultT{};

		if (nextPart != s.end() && *nextPart == '.') {
			const static std::string exponentDelimiters = "eE";
			auto fractionalEnd = std::ranges::find_first_of(nextPart, s.end(), exponentDelimiters.begin(), exponentDelimiters.end());
			std::string_view fractionalStr{ std::next(nextPart), fractionalEnd };
			nextPart = fractionalEnd;

			ResultT digitValue{ 1 };
			for (char c : fractionalStr) {
				if (!isDigit(c))
					throw NotADigitError{ c, s };

				digitValue /= 10;
				result += digitValue * (c - '0');
			}
		}

		if (nextPart != s.end())
			result *= std::pow(10, parseInt<ResultT>({ std::next(nextPart), s.end() }));

		return result;
	}

	/// Iterates over line from s
	template <typename Callback> requires std::invocable<Callback, std::string_view>
	void forEachLine(std::string_view s, Callback&& callback) {
		auto lineStart = s.begin();
		while (lineStart != s.end()) {
			auto lineEnd = std::ranges::find(lineStart, s.end(), '\n');
			callback(std::string_view{lineStart, lineEnd});

			if (lineEnd == s.end()) {
				break;
			}
			lineStart = std::next(lineEnd);
		}
	}

	/// @brief Iterates over stripped line from is
	/// @details Comments and spaces are stripped.
	/// Empty lines are ignored.
	/// Rest is passed to callback with the line number (skipped lines aren't counted).
	template <typename Callback> requires std::invocable<Callback, std::string_view, int>
	void forEachStrippedLine(std::istream& is, Callback&& callback) {
		std::string line;
		int index = 0;
		while (std::getline(is, line))
			if (std::string_view meaningful = strip(stripComment(line)); !meaningful.empty())
				callback(meaningful, index++);
	}

	/// Strips each line in is and passes it to the callback
	template <typename Callback> requires std::invocable<Callback, std::string_view>
	void forEachStrippedLine(std::istream& is, Callback&& callback) {
		forEachStrippedLine(is, [&callback](std::string_view line, [[maybe_unused]] int index) {
			callback(line);
		});
	}

	/// Value not given
	class NoValueError : public ParseError {
	public:
		NoValueError(std::string_view key, std::string_view parsed, Stacktrace currentStacktrace = {}) noexcept :
			ParseError{ std::format("No value given for key \"{}\"", key), parsed, std::move(currentStacktrace) } {}
	};

	/// @brief Splits string into key and value
	/// @details Key is part before first space.
	/// Values is part after first continuous space block.
	/// Returns subview of the given view.
	inline std::pair<std::string_view, std::string_view> parseKeyValuePair(std::string_view s) {
		if (s.empty())
			throw EmptyStringError{ s };

		auto keyEnd = std::ranges::find_if(s.begin(), s.end(), isSpace);
		std::string_view key{ s.begin(), keyEnd };

		auto valueStart = std::ranges::find_if_not(keyEnd, s.end(), isSpace);
		if (valueStart == s.end())
			throw NoValueError{ key, s };
		std::string_view value{ valueStart, s.end()};

		return { key, value };
	}

	/// Parses whole stream as key-value pairs
	inline std::unordered_map<std::string, std::string> parseMapping(std::istream& is) {
		std::unordered_map<std::string, std::string> result;
		forEachStrippedLine(is, [&result](std::string_view line) {
			result.insert(parseKeyValuePair(line));
		});
		return result;
	}

	/// String required to be true of false isn't
	class NotABoolError : public ParseError {
	public:
		NotABoolError(std::string_view parsed, Stacktrace currentStacktrace = {}) noexcept :
			ParseError{"expected true or false", parsed, std::move(currentStacktrace)} {}
	};

	inline bool parseBool(std::string_view s) {
		if (s.empty())
			throw EmptyStringError{s};

		if (s == "true")
			return true;
		else if (s == "false")
			return false;
		else
			throw NotABoolError(s);
	}

	/// @brief Parses comma-separated list
	/// @detail Values are separated by commas.
	/// Commas aren't parts of values.
	/// Trailing commas are ignored (trailing commas before ws aren't).
	/// Empty string gives empty list.
	inline std::vector<std::string_view> parseList(std::string_view s) {
		std::vector<std::string_view> res;
		auto prev = s.begin();
		while (prev != s.end()) {
			auto next = std::ranges::find(prev, s.end(), ',');

			res.emplace_back(prev, next);

			if (next == s.end())
				break;
			++next;
			prev = next;
		}
		return res;
	}

	/// @brief Parses 3D char map
	/// @detail New line ends a row.
	/// Line containing at least 1 - and spaces ends a level.
	/// Spaces at the end of a row or level are ignored.
	/// Spaces in the begining or in the middle aren't.
	/// Spaces are added to make shape a rectangular cuboid.
	inline Array3D<char> parseCharMap(std::string_view s) {
		int maxWidth = 0;
		int maxHeight = 0;

		int meaningfulHeight = 0;
		int height = 0;
		int depth = 1;
		forEachLine(s, [&](std::string_view line) {
			int width = std::ssize(rightStrip(line));
			if (width == 0) {
				++height;
				return;
			}

			if (std::ranges::all_of(line, [](char c) {
				return c == '-' || isSpace(c);
			})) {
				maxHeight = std::max(meaningfulHeight, maxHeight);
				height = 0;
				meaningfulHeight = 0;

				++depth;

				return;
			}
			
			maxWidth = std::max(width, maxWidth);
			++height;
			meaningfulHeight = height;
		});

		maxHeight = std::max(meaningfulHeight, maxHeight);

		Array3D<char> result;
		result.assign({maxWidth, maxHeight, depth}, ' ');

		sf::Vector3i pos;
		forEachLine(s, [&](std::string_view line) {
			auto meaningful = rightStrip(line);

			if (!meaningful.empty() && std::ranges::all_of(line, [](char c) {
				return c == '-' || isSpace(c);
			})) {
				pos.y = 0;
				++pos.z;
				return;
			}

			for (char c : meaningful) {
				result[pos] = c;
				++pos.x;
			}

			pos.x = 0;
			++pos.y;
		});

		return result;
	}
}

#endif