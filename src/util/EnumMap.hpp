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

#ifndef ENUM_MAP_HPP_
#define ENUM_MAP_HPP_

#include "enum.hpp"
#include "assert.hpp"

#include <concepts>
#include <type_traits>

namespace util {
	template <typename Enum, typename T>
	class EnumMap {
	public:
		EnumMap() = default;

		/// @warning Check key by yourself
		[[nodiscard]] T& operator[] (Enum key) {
			TROTE_ASSERT(0 <= static_cast<int>(key) && static_cast<int>(key) < nEnumerators<Enum>);
			return elements[static_cast<int>(key)];
		}

		/// @warning Check key by yourself
		[[nodiscard]] const T& operator[] (Enum key) const {
			TROTE_ASSERT(0 <= static_cast<int>(key) && static_cast<int>(key) < nEnumerators<Enum>);
			return elements[static_cast<int>(key)];
		}

		/// Sets all values to given value
		void fill(T value) {
			elements.fill(value);
		}

		template <std::invocable<T> Proj>
		auto transform(Proj&& proj) const {
			EnumMap<std::invoke_result_t<Proj, T>> result;
			for (int i = 0; i < std::ssize(elements); ++i) {
				result.elements[i] = proj(elements[i]);
			}
			return result;
		}
	private:
		std::array<T, nEnumerators<Enum>> elements;
	};
}

#endif