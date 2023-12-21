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

#ifndef PATHFINDING_HPP_
#define PATHFINDING_HPP_

#include "core/fwd.hpp"
#include "core/World.hpp"

#include "Array3D.hpp"
#include "Direction.hpp"
#include "assert.hpp"

#include <SFML/System/Vector3.hpp>

#include <optional>
#include <queue>

namespace util {
	struct PathBuffer {
		struct Node {
			int distance = std::numeric_limits<int>::max();
			sf::Vector3i prevOffset{0, 0, 0};
			bool onPath = false;
		};

		util::Array3D<Node> buffer;
		std::optional<sf::Vector3i> lastTarget;
	};

	struct PathUpdate {
		PathUpdate(int distance_, sf::Vector3i position_, sf::Vector3i target, sf::Vector3i prevOffset_) noexcept :
			distance{distance_}, minFullDistance{distance_ + util::uniformDistance(position_, target)},
			position{position_}, prevOffset{prevOffset_} {}

		int distance;
		int minFullDistance;
		sf::Vector3i position;
		sf::Vector3i prevOffset;

		friend auto operator <=> (const PathUpdate& lhs, const PathUpdate& rhs) noexcept {
			return lhs.minFullDistance <=> rhs.minFullDistance;
		}
	};

	template <typename IsPassable>
		requires std::convertible_to<std::invoke_result_t<IsPassable, const core::World&, sf::Vector3i>, bool>
	void findPath(const core::World& world, sf::Vector3i from, sf::Vector3i to, 
				  util::Array3D<PathBuffer::Node>& buffer, const IsPassable& isPassable) {
		buffer.assign(world.tiles().shape(), {});

		std::priority_queue<PathUpdate, std::vector<PathUpdate>, std::greater<>> queue;
		queue.emplace(0, from, to, sf::Vector3i{0, 0, 0});

		while (!queue.empty()) {
			PathUpdate update = queue.top();
			queue.pop();

			TROTE_ASSERT(world.tiles().isValidPosition(update.position));

			if (update.minFullDistance >= buffer[to].distance)
				break;

			if (update.distance >= buffer[update.position].distance)
				continue;

			buffer[update.position].distance = update.distance;
			buffer[update.position].prevOffset = update.prevOffset;

			for (sf::Vector2i direction : util::nonzeroDirections<int>) {
				sf::Vector3i direction3D = util::make3D(direction, 0);
				sf::Vector3i nextPos = update.position + direction3D;
				if (world.tiles().isValidPosition(nextPos) && isPassable(world, nextPos)) 
					queue.emplace(update.distance + 1, nextPos, to, -direction3D);
			}

			if (auto destination = world.upStairs(update.position))
				queue.emplace(update.distance + 1, *destination, to, update.position - *destination);

			if (auto destination = world.downStairs(update.position))
				queue.emplace(update.distance + 1, *destination, to, update.position - *destination);
		}
	}

	/// @brief Computes next move to perform to move from position to target
	/// @param buffer Buffer reused to prevent allocating memory
	template <typename IsPassable>
		requires std::convertible_to<std::invoke_result_t<IsPassable, const core::World&, sf::Vector3i>, bool>
	sf::Vector3i nextStep(const core::World& world, sf::Vector3i position, sf::Vector3i target,
						  PathBuffer& buffer, const IsPassable& isPassable) {
		TROTE_ASSERT(world.tiles().isValidPosition(position));
		TROTE_ASSERT(world.tiles().isValidPosition(target));

		if (!buffer.lastTarget || *buffer.lastTarget != target || !buffer.buffer[position].onPath) {
			findPath(world, position, target, buffer.buffer, isPassable);
			buffer.lastTarget = target;
		}

		buffer.buffer[position].onPath = true;

		sf::Vector3i current = target;
		while (true) {
			buffer.buffer[current].onPath = true;

			sf::Vector3i prevOffset = buffer.buffer[current].prevOffset;

			if (current + prevOffset == position)
				return -buffer.buffer[current].prevOffset;

			if (prevOffset == sf::Vector3i{0, 0, 0})
				return {0, 0, 0};

			current += prevOffset;
		}

		TROTE_ASSERT(false, "unreachable");
	}

	template <typename IsPassable>
		requires std::convertible_to<std::invoke_result_t<IsPassable, core::Tile>, bool>
	sf::Vector3i nextStep(const core::World& world, sf::Vector3i position, sf::Vector3i target,
		                  PathBuffer& buffer, const IsPassable& isPassable) {
		return nextStep(world, position, target, buffer, [&](const core::World& world, sf::Vector3i pos) {
			return isPassable(world.tiles()[pos]);
		});
	}

	inline sf::Vector3i nextStep(const core::World& world, sf::Vector3i position, sf::Vector3i target,
		                  PathBuffer& buffer) {
		return nextStep(world, position, target, buffer, &core::isPassable);
	}

	struct ExplorePathUpdate {
		int distance;
		sf::Vector3i position;
		sf::Vector3i prevOffset;

		friend auto operator <=> (const ExplorePathUpdate& lhs, const ExplorePathUpdate& rhs) noexcept {
			return lhs.distance <=> rhs.distance;
		}
	};

	/// @brief finds path (may be not shortest) to a tile satisfying isTarget
	/// @returns lastTarget to found target position
	template <typename IsTarget>
		requires std::convertible_to<std::invoke_result_t<IsTarget, const core::World&, sf::Vector3i>, bool>
	std::optional<sf::Vector3i> findExplorePath(const core::World& world, sf::Vector3i from, 
		             util::Array3D<PathBuffer::Node>& buffer, const IsTarget& isTarget) {
		buffer.assign(world.tiles().shape(), {});

		std::priority_queue<ExplorePathUpdate, std::vector<ExplorePathUpdate>, std::greater<>> queue;
		queue.emplace(0, from, sf::Vector3i{0, 0, 0});

		while (!queue.empty()) {
			ExplorePathUpdate update = queue.top();
			queue.pop();

			TROTE_ASSERT(world.tiles().isValidPosition(update.position));

			if (update.distance >= buffer[update.position].distance)
				continue;

			buffer[update.position].distance = update.distance;
			buffer[update.position].prevOffset = update.prevOffset;

			if (isTarget(world, update.position))
				return update.position;

			for (sf::Vector2i direction : util::nonzeroDirections<int>) {
				sf::Vector3i direction3D = util::make3D(direction, 0);
				sf::Vector3i nextPos = update.position + direction3D;
				if (world.tiles().isValidPosition(nextPos) && isPassable(world.tiles()[nextPos]))
					queue.emplace(update.distance + 1, nextPos, -direction3D);
			}

			if (auto destination = world.upStairs(update.position))
				queue.emplace(update.distance + 1, *destination, update.position - *destination);

			if (auto destination = world.downStairs(update.position))
				queue.emplace(update.distance + 1, *destination, update.position - *destination);
		}

		return std::nullopt;
	}

	/// @brief Computes next move to tile satisfying isTarget
	/// @param buffer Buffer reused to prevent allocating memory
	template <typename IsTarget>
		requires std::convertible_to<std::invoke_result_t<IsTarget, const core::World&, sf::Vector3i>, bool>
	std::optional<sf::Vector3i> nextExploreStep(const core::World& world, sf::Vector3i position,
		                                        PathBuffer& buffer, const IsTarget& isTarget) {
		TROTE_ASSERT(world.tiles().isValidPosition(position));

		if (!buffer.lastTarget || !isTarget(world, *buffer.lastTarget) || !buffer.buffer[position].onPath) {
			if (auto target = findExplorePath(world, position, buffer.buffer, isTarget))
				buffer.lastTarget = target;
			else
				return std::nullopt;
		}

		buffer.buffer[position].onPath = true;

		sf::Vector3i current = *buffer.lastTarget;
		while (true) {
			buffer.buffer[current].onPath = true;

			sf::Vector3i prevOffset = buffer.buffer[current].prevOffset;

			if (current + prevOffset == position)
				return -buffer.buffer[current].prevOffset;

			if (prevOffset == sf::Vector3i{0, 0, 0})
				return sf::Vector3i{0, 0, 0};

			current += prevOffset;
		}

		TROTE_ASSERT(false, "unreachable");
	}
}

#endif