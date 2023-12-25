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

#ifndef PARSE_KEY_VALUE_HPP_
#define PARSE_KEY_VALUE_HPP_

#include "parse.hpp"
#include "Map.hpp"

#include <functional>

namespace util {
	/// Value not given
	class NoValueError : public ParseError {
	public:
		NoValueError(std::string_view key, std::string_view parsed, Stacktrace currentStacktrace = {}) noexcept :
			ParseError{std::format("No value given for key \"{}\"", key), parsed, std::move(currentStacktrace)} {}
	};

	/// @brief Splits string into key and value
	/// @details Key is part before first space.
	/// Values is part after first continuous space block.
	/// Returns subviews of the given view.
	inline std::pair<std::string_view, std::string_view> parseKeyValuePair(std::string_view s) {
		if (s.empty())
			throw EmptyStringError{s};

		auto keyEnd = std::ranges::find_if(s.begin(), s.end(), isSpace);
		std::string_view key{s.begin(), keyEnd};

		auto valueStart = std::ranges::find_if_not(keyEnd, s.end(), isSpace);
		std::string_view value{valueStart, s.end()};

		return {key, value};
	}

	/// Parses whole source as key-value pairs and forwards to callback
	template <std::invocable<std::string_view, std::string_view> Callback>
	inline void forEackKeyValuePair(auto&& source, Callback&& callback) {
		forEachStrippedLine(source, [&callback](std::string_view line) {
			auto [key, value] = parseKeyValuePair(line);
			callback(key, value);
		});
	}

	/// Parses whole source as key-value pairs
	inline std::unordered_map<std::string, std::string> parseMapping(auto&& source) {
		std::unordered_map<std::string, std::string> result;
		forEackKeyValuePair(source, [&result](std::string_view key, std::string_view value) {
			result.emplace(key, value);
		});
		return result;
	}

	class FirstHeaderExpected : public util::RuntimeError {
	public:
		FirstHeaderExpected(util::Stacktrace stacktrace = {}) :
			util::RuntimeError{"Header expected in the begining of sectioned data", stacktrace} {}
	};

	class ClosingBracketNotFound : public util::RuntimeError {
	public:
		ClosingBracketNotFound(util::Stacktrace stacktrace = {}) :
			util::RuntimeError{"Expected closing square bracket", stacktrace} {}
	};

	/// @brief Iterates over sections in s
	/// @details sections are delimited by headers.
	/// Header is a section name in square brackets;
	/// If line after header contains only spaces it's ignored;
	template <std::invocable<std::string_view, std::string_view> Callback>
	void forEachSection(std::string_view s, Callback&& callback) {
		if (s.empty())
			return;

		if (s.front() != '[')
			throw FirstHeaderExpected{};

		auto sectionStart = s.begin();
		while (sectionStart != s.end()) {
			auto nameStart = std::next(sectionStart);
			auto nameEnd = std::ranges::find(nameStart, s.end(), ']');

			if (nameEnd == s.end()) {
				throw ClosingBracketNotFound{};
			}

			auto dataStart = std::next(nameEnd);

			auto lineEnd = std::ranges::find(nameEnd, s.end(), '\n');
			if (std::ranges::all_of(dataStart, lineEnd, &isSpace)) {
				if (lineEnd == s.end()) {
					dataStart = s.end();
				} else {
					dataStart = std::next(lineEnd);
				}
			}

			auto dataEnd = std::ranges::find(dataStart, s.end(), '[');

			callback(std::string_view{nameStart, nameEnd}, {dataStart, dataEnd});

			sectionStart = dataEnd;
		}
	}

	/// Parses whole source as key-value pairs separated by commas and forwards to callback
	template <std::invocable<std::string_view, std::string_view> Callback>
	inline void forEackInlineKeyValuePair(auto&& source, Callback&& callback) {
		for (std::string_view pair : util::parseList(source)) {
			auto [key, value] = parseKeyValuePair(strip(pair));
			callback(key, value);
		}
	}

	/// Helper for defining callbacks for processing key value pairs
	class KeyValueVisitor {
	public:
		class Action {
		public:
			class DuplicateKey : public RuntimeError {
			public:
				DuplicateKey(Stacktrace stacktrace = {}) :
					RuntimeError("Duplicate for some unique key encountered", std::move(stacktrace)) {}
			};

			class RequiredKey : public RuntimeError {
			public:
				RequiredKey(Stacktrace stacktrace = {}) :
					RuntimeError("Value for some required key not given", std::move(stacktrace)) {}
			};

			Action& callback(std::function<void(std::string_view)> newCallback) {
				callback_ = std::move(newCallback);
				return *this;
			}

			Action& unique() noexcept {
				isUnique = true;
				return *this;
			}

			Action& required() noexcept {
				isRequired = true;
				return *this;
			}

			void operator() (std::string_view data) {
				if (isUnique && wasVisited) {
					throw DuplicateKey{};
				}

				wasVisited = true;
				callback_(data);
			}

			void validate() {
				if (isRequired && !wasVisited) {
					throw RequiredKey{};
				}
			}
		private:
			std::function<void(std::string_view)> callback_;

			bool isUnique = false;
			bool isRequired = false;
			bool wasVisited = false;
		};

		Action& key(const std::string& key_) {
			return actions[key_];
		}

		class UnknownKey : RuntimeError {
		public:
			UnknownKey(std::string_view key_, Stacktrace stacktrace = {}) :
				RuntimeError(std::format("Key \"{}\" is unknown", key_), std::move(stacktrace)) {}
		};

		void operator() (std::string_view key_, std::string_view data) {
			if (auto iter = actions.find(std::string{key_}); iter != actions.end()) {
				iter->second(data);
			} else {
				throw UnknownKey{key_};
			}
		}

		void validate() {
			for (auto& [key_, action] : actions) {
				action.validate();
			}
		}
	private:
		std::unordered_map<std::string, Action> actions;
	};
}

#endif