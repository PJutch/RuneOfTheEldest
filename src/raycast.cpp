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

#include "geometry.hpp"

bool canSee(sf::Vector3i pos1, sf::Vector3i pos2, const World& world) {
	if (pos1 == pos2)
		return true;

	if (pos1.z != pos2.z)
		return false;

	double distance_ = distance(getXY(pos1), getXY(pos2));

	// sin and cos of angle between pos2 - pos1 and x axis
	double cos = (pos2.x - pos1.x) / distance_;
	double sin = (pos2.y - pos1.y) / distance_;

	double wholeDistance;
	double offset = std::modf(distance_, &wholeDistance) / 2 + 1; // offset to make checked positions simmetrical

	// checks points on line from pos1 to pos2 with step 1
	for (double distance = offset; distance <= distance_ - offset; ++distance) {
		int x = pos1.x + std::round(distance * cos);
		int y = pos1.y + std::round(distance * sin);

		if (!isPassable(world.tiles()[{x, y, pos1.z}]))
			return false;
	}

	return true;
}
