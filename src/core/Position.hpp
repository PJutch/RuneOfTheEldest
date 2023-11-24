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

#ifndef POSITION_HPP_
#define POSITION_HPP_

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

namespace core {
	/// Position in World
	template <typename T>
	struct Position {
		T x;
		T y;
		int z;

		Position() = default;
		Position(T x_, T y_, int z_) : x{x_}, y{y_}, z{z_} {}
		Position(sf::Vector2<T> xy_, int z_) : x{xy_.x}, y{xy_.y}, z{z_} {}
		explicit Position(sf::Vector3i position) : 
			Position(static_cast<T>(position.x), static_cast<T>(position.y), position.z) {}

		/// Horizontal components
		sf::Vector2<T> xy() const noexcept {
			return {x, y};
		}

		template <typename T>
		explicit operator sf::Vector3<T>() {
			return {static_cast<T>(x), static_cast<T>(y), static_cast<T>(z)};
		}
	};
}

#endif