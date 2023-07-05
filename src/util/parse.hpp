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

	template <typename Callback> requires std::invocable<Callback, const std::string_view&, int>
	void forEachStrippedLine(std::istream& is, Callback&& callback) {
		std::string line;
		int index = 0;
		while (std::getline(is, line))
			callback(strip(line), index ++);
	}

	template <typename Callback> requires std::invocable<Callback, const std::string_view&>
	void forEachStrippedLine(std::istream& is, Callback&& callback) {
		forEachStrippedLine(is, [&callback](const std::string_view& line, int index) {
			callback(line);
		});
	}
}

#endif