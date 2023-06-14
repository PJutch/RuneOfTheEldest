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

#include "Dungeon.hpp"

#include "geometry.hpp"

#include <queue>

namespace {
	struct PathNode {
		int distance;
		sf::Vector3i prevOffset;
	};

	class PathBuffer {
	public:
		PathBuffer(const Dungeon& dungeon) {
			buffer.reserve(dungeon.size());
			for (int level = 0; level < dungeon.size(); ++level)
				buffer.emplace_back(dungeon[level].shape());
		}

		PathNode& at(sf::Vector3i position) noexcept {
			return buffer[position.z].at(getXY(position));
		}

		const PathNode& at(sf::Vector3i position) const noexcept {
			return buffer[position.z].at(getXY(position));
		}
	private:
		class LevelPathBuffer {
		public:
			LevelPathBuffer(sf::Vector2i shape_) : shape{ shape_ } {
				buffer.resize(shape.x * shape.y, { std::numeric_limits<int>::max(), sf::Vector3i{0, 0, 0} });
			}

			PathNode& at(sf::Vector2i position) noexcept {
				return buffer.at(position.x * shape.y + position.y);
			}

			const PathNode& at(sf::Vector2i position) const noexcept {
				return buffer.at(position.x * shape.y + position.y);
			}
		private:
			std::vector<PathNode> buffer;
			sf::Vector2i shape;
		};

		std::vector<LevelPathBuffer> buffer;
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

	PathBuffer findPath(const Dungeon& dungeon, sf::Vector3i from, sf::Vector3i to) {
		PathBuffer buffer(dungeon);

		std::priority_queue<PathUpdate, std::vector<PathUpdate>, std::greater<>> queue;
		queue.emplace(0, from, to, sf::Vector3i{0, 0, 0});

		while (!queue.empty()) {
			PathUpdate update = queue.top();
			queue.pop();

			if (update.minFullDistance >= buffer.at(to).distance)
				break;

			if (update.distance >= buffer.at(update.position).distance)
				continue;

			buffer.at(update.position).distance = update.distance;
			buffer.at(update.position).prevOffset = update.prevOffset;

			for (sf::Vector2i direction : directions<int>) {
				sf::Vector3i direction3D = make3D(direction, 0);
				sf::Vector3i nextPos = update.position + direction3D;
				if (dungeon.isValidPosition(nextPos) && isPassable(dungeon.at(nextPos)))
					queue.emplace(update.distance + 1, nextPos, to, -direction3D);
			}

			if (auto destination = dungeon.upStairs(update.position))
				queue.emplace(update.distance + 1, *destination, to, update.position - *destination);

			if (auto destination = dungeon.downStairs(update.position))
				queue.emplace(update.distance + 1, *destination, to, update.position - *destination);
		}

		return buffer;
	}
}

sf::Vector3i nextStep(const Dungeon& dungeon, sf::Vector3i position, sf::Vector3i target) {
	PathBuffer path = findPath(dungeon, position, target);

	sf::Vector3i current = target;
	while (true) {
		sf::Vector3i prevOffset = path.at(current).prevOffset;

		if (current + prevOffset == position)
			return -path.at(current).prevOffset;

		if (prevOffset == sf::Vector3i{0, 0, 0})
			return { 0, 0, 0 };

		current += prevOffset;
	}

	TROTE_ASSERT(false, "unreachable");
}
