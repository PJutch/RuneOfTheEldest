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

#include "pathfinding.hpp"

#include "World.hpp"

#include "Array3D.hpp"
#include "geometry.hpp"
#include "assert.hpp"

#include <queue>

namespace {
	struct PathNode {
		int distance = std::numeric_limits<int>::max();
		sf::Vector3i prevOffset{ 0, 0, 0 };
	};

	struct PathUpdate {
		PathUpdate(int distance_, sf::Vector3i position_, sf::Vector3i target, sf::Vector3i prevOffset_) noexcept :
			distance{ distance_ }, minFullDistance{ distance_ + uniformDistance(position_, target) },
			position{ position_ }, prevOffset{ prevOffset_ } {}

		int distance;
		int minFullDistance;
		sf::Vector3i position;
		sf::Vector3i prevOffset;

		friend auto operator <=> (const PathUpdate& lhs, const PathUpdate& rhs) noexcept {
			return lhs.minFullDistance <=> rhs.minFullDistance;
		}
	};

	Array3D<PathNode> findPath(const World& world, sf::Vector3i from, sf::Vector3i to) {
		const Dungeon& dungeon = world.dungeon();

		Array3D<PathNode> buffer;
		buffer.assign(dungeon.shape());

		std::priority_queue<PathUpdate, std::vector<PathUpdate>, std::greater<>> queue;
		queue.emplace(0, from, to, sf::Vector3i{0, 0, 0});

		while (!queue.empty()) {
			PathUpdate update = queue.top();
			queue.pop();

			if (update.minFullDistance >= buffer[to].distance)
				break;

			if (update.distance >= buffer[update.position].distance)
				continue;

			buffer[update.position].distance = update.distance;
			buffer[update.position].prevOffset = update.prevOffset;

			for (sf::Vector2i direction : directions<int>) {
				sf::Vector3i direction3D = make3D(direction, 0);
				sf::Vector3i nextPos = update.position + direction3D;
				if (dungeon.isValidPosition(nextPos) && isPassable(dungeon[nextPos]))
					queue.emplace(update.distance + 1, nextPos, to, -direction3D);
			}

			if (auto destination = world.upStairs(update.position))
				queue.emplace(update.distance + 1, *destination, to, update.position - *destination);

			if (auto destination = world.downStairs(update.position))
				queue.emplace(update.distance + 1, *destination, to, update.position - *destination);
		}

		return buffer;
	}
}

sf::Vector3i nextStep(const World& world, sf::Vector3i position, sf::Vector3i target) {
	Array3D<PathNode> path = findPath(world, position, target);

	sf::Vector3i current = target;
	while (true) {
		sf::Vector3i prevOffset = path[current].prevOffset;

		if (current + prevOffset == position)
			return -path[current].prevOffset;

		if (prevOffset == sf::Vector3i{0, 0, 0})
			return { 0, 0, 0 };

		current += prevOffset;
	}

	TROTE_ASSERT(false, "unreachable");
}
