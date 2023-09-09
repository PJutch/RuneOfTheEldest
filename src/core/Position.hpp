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
		Position(float x_, float y_, int z_) : x{x_}, y{y_}, z{z_} {}
		explicit Position(sf::Vector3i position) : Position(static_cast<float>(position.x),
			static_cast<float>(position.y),
			position.z) {}

		/// Horizontal components
		sf::Vector2f xy() const noexcept {
			return {x, y};
		}
	};
}

#endif