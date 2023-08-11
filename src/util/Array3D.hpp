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

#ifndef ARRAY_3D_HPP_
#define ARRAY_3D_HPP_

#include "geometry.hpp"
#include "assert.hpp"

#include <vector>

namespace util {
	template <typename T>
	class Array3D {
	public:
		Array3D() = default;

		/// Checks if there are elements with this x
		[[nodiscard]] bool isValidX(int x) const noexcept {
			return 0 <= x && x < shape().x;
		}

		/// Checks if there are elements with this y
		[[nodiscard]] bool isValidY(int y) const noexcept {
			return 0 <= y && y < shape().y;
		}

		/// Checks if there are elements with this z
		[[nodiscard]] bool isValidZ(int z) const noexcept {
			return 0 <= z && z < shape().z;
		}

		/// Checks if position is valid element position
		[[nodiscard]] bool isValidPosition(sf::Vector3i position) const noexcept {
			return isValidZ(position.z) && isValidX(position.x) && isValidY(position.y);
		}

		/// Checks if rect sides are >=0 and all elements in it are exists
		[[nodiscard]] bool isValidRect(sf::IntRect rect) const noexcept {
			return isValidX(rect.left) && isValidY(rect.top)
				&& rect.width >= 0 && rect.height >= 0
				&& isValidX(rect.left + rect.width - 1)
				&& isValidY(rect.top + rect.height - 1);
		}

		/// @warning Check position by yourself
		[[nodiscard]] T& operator[] (sf::Vector3i position) noexcept {
			TROTE_ASSERT(isValidPosition(position));
			auto pos = util::geometry_cast<ptrdiff_t>(position);
			return elements[pos.z * shape_.x * shape_.y + pos.x * shape_.y + pos.y];
		}

		/// @warning Check position by yourself
		[[nodiscard]] const T& operator[] (sf::Vector3i position) const noexcept {
			TROTE_ASSERT(isValidPosition(position));
			auto pos = util::geometry_cast<ptrdiff_t>(position);
			return elements[pos.z * shape_.x * shape_.y + pos.x * shape_.y + pos.y];
		}

		/// Array sizes in all axis
		sf::Vector3i shape() const noexcept {
			return shape_;
		}

		/// Array bounding rect in XY axis
		sf::IntRect horizontalBounds() const noexcept {
			return { {0, 0}, getXY(shape()) };
		}

		/// Clears and creates new dungeon with given shape filled with given value
		void assign(sf::Vector3i newShape, T value) {
			TROTE_ASSERT(newShape.x >= 0);
			TROTE_ASSERT(newShape.y >= 0);
			TROTE_ASSERT(newShape.z >= 0);

			elements.clear();
			shape_ = newShape;
			elements.resize(static_cast<ptrdiff_t>(shape().x)
				          * static_cast<ptrdiff_t>(shape().y)
				          * static_cast<ptrdiff_t>(shape().z), value);
		}
	private:
		std::vector<T> elements;
		sf::Vector3i shape_;
	};
}

#endif