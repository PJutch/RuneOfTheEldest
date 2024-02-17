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

#ifndef ENUM_HPP_
#define ENUM_HPP_

#include "util/case.hpp"

#include <boost/describe.hpp>
#include <boost/mp11.hpp>

#include <unordered_map>
#include <span>
#include <string>
#include <type_traits>

namespace util {
	template <typename T>
	std::unordered_map<std::string, T> nameMapFromNames(std::span<const std::string> names) {
		std::unordered_map<std::string, T> res;
		for (int i = 0; i < std::ssize(names); ++i)
			res[names[i]] = T{i};
		return res;
	}

	template <typename E>
	std::unordered_map<std::string, E> makeNameMap() {
		std::unordered_map<std::string, E> res;
		using Values = boost::describe::describe_enumerators<E>;
		boost::mp11::mp_for_each<Values>([&](auto D) {
			res[util::toLower(D.name)] = D.value;
		});
		return res;
	}

	template <typename T> requires std::is_enum_v<T>
	inline const int nEnumerators = nEnumeratorsImpl(std::type_identity<T>{});

	template <typename Enum> requires std::is_enum_v<Enum>
	std::string enumeratorName(Enum enumerator) {
		std::string result;
		boost::mp11::mp_for_each<boost::describe::describe_enumerators<Enum>>([&](auto D) {
			if (D.value == enumerator) {
				result = D.name;
			}
		});
		return result;
	}
}

/// returns 10 argument
#define TROTE_ARG_10(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ...) a10

/// returns number of arguments
#define TROTE_N_ARGS(...) TROTE_ARG_10(__VA_ARGS__ __VA_OPT__(,) 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

/// defines enum, boost.describe description and 
#define TROTE_DEFINE_ENUM(name, ...) \
	BOOST_DEFINE_ENUM(name __VA_OPT__(,) __VA_ARGS__) \
    constexpr int nEnumeratorsImpl(std::type_identity<name>) { \
		return TROTE_N_ARGS(__VA_ARGS__); \
	}

#endif