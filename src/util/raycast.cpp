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

#include "raycast.hpp"

#include "core/World.hpp"

#include "geometry.hpp"

namespace util {
	namespace {
		/// Checks if position1 and position2 are visible from each other
		bool isObstructed(sf::Vector2<double> pos1, sf::Vector2<double> pos2, int z, const core::World& world) {
			if (pos1 == pos2)
				return false;

			double distance_ = util::distance(pos1, pos2);

			// sin and cos of angle between pos2 - pos1 and x axis
			double cos = (pos2.x - pos1.x) / distance_;
			double sin = (pos2.y - pos1.y) / distance_;

			double wholeDistance;
			double offset = std::modf(distance_, &wholeDistance) / 2; // offset to make checked positions simmetrical

			// checks points on line from pos1 to pos2 with step 1
			for (double distance = offset; distance <= distance_ - offset; ++distance) {
				int x = pos1.x + std::round(distance * cos);
				int y = pos1.y + std::round(distance * sin);

				if (!isPassable(world.tiles()[sf::Vector3i(x, y, z)]))
					return true;
			}

			return false;
		}

		bool canSee(sf::Vector2i from, sf::Vector2i to, int z, const core::World& dungeon) {
			const std::array<sf::Vector2<double>, 5> toCheck{ sf::Vector2<double>{-0.5, -0.5},
																				 { 0.5, -0.5},
																				 {-0.5,  0.5},
																				 { 0.5,  0.5},
																				 { 0.0,  0.0} };

			for (sf::Vector2<double> fromOffset : toCheck)
				for (sf::Vector2<double> toOffset : toCheck)
					if (!isObstructed(geometry_cast<double>(from) + fromOffset,
						              geometry_cast<double>(to) + toOffset, z, dungeon))
						return true;

			return false;
		}
	}

	bool Raycaster::canSee(sf::Vector3i from, sf::Vector3i to) {
		if (from == to)
			return true;

		if (from.z != to.z)
			return false;

		auto from2D = getXY(from);
		auto to2D = getXY(to);
		int z = from.z;

		auto cache_iter = cache.find({ from2D, to2D, z });
		if (cache_iter != cache.end())
			return cache_iter->second;
		
		bool result = util::canSee(from2D, to2D, z, *world);
		cache[{ from2D, to2D, z }] = result;
		return result;
	}
}
