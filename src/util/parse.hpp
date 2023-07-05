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

#include <format>
#include <iostream>
#include <algorithm>
#include <string>
#include <string_view>
#include <concepts>
#include <cctype>

namespace util {
	/// More safe wrapper around isspace
	bool isSpace(char c) noexcept {
		return std::isspace(static_cast<unsigned char>(c));
	}

	/// More safe wrapper around isdigit
	bool isDigit(char c) noexcept {
		return std::isdigit(static_cast<unsigned char>(c));
	}

	/// @brief Strip spaces from left and right
	/// @details Removes all chars satisfying isSpace from left and right.
	/// Returns subview of input view
	std::string_view strip(std::string_view s) noexcept {
		auto begin = std::ranges::find_if_not(s, &isSpace);
		if (begin == s.end())
			return {};

		auto end = std::ranges::find_if_not(s.rbegin(), s.rend(), &isSpace).base();
		return { begin, end };
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
	template <typename ResultT>
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
	template <typename ResultT>
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
	template <typename ResultT>
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

	/// Strips each line in is and passes it to the callback with line index
	template <typename Callback> requires std::invocable<Callback, std::string_view, int>
	void forEachStrippedLine(std::istream& is, Callback&& callback) {
		std::string line;
		int index = 0;
		while (std::getline(is, line))
			callback(strip(line), index ++);
	}

	/// Strips each line in is and passes it to the callback
	template <typename Callback> requires std::invocable<Callback, std::string_view>
	void forEachStrippedLine(std::istream& is, Callback&& callback) {
		forEachStrippedLine(is, [&callback](std::string_view line, int index) {
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
	std::pair<std::string_view, std::string_view> parseKeyValuePair(std::string_view s) {
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
	std::unordered_map<std::string, std::string> parseMapping(std::istream& is) {
		std::unordered_map<std::string, std::string> result;
		forEachStrippedLine(is, [&result](std::string_view line) {
			result.insert(parseKeyValuePair(line));
		});
		return result;
	}
}

#endif