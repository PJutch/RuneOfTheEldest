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

#ifndef RAYCAST_HPP_
#define RAYCAST_HPP_

#include "core/fwd.hpp"

#include <util/geometry.hpp>
#include <util/Map.hpp>

#include <SFML/System/Vector3.hpp>

#include <memory>
#include <tuple>

namespace util {
	class Raycaster {
	public:
		Raycaster(std::shared_ptr<core::World> world_) :
			world{ std::move(world_) } {}

		/// Checks if tile at to can be seen from tile at from
		bool canSee(sf::Vector3i from, sf::Vector3i to);
	private:
		std::shared_ptr<core::World> world;

		UnorderedMap<std::tuple<sf::Vector2i, sf::Vector2i, int>, bool> cache;
	};
}

#endif