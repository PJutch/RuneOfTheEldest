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

#include "Goblin.hpp"

#include "pathfinding.hpp"
#include "raycast.hpp"
#include "geometry.hpp"

bool Goblin::act() {
	updateTarget();
	travelToTarget();
	wait(1);
	return true;
}

void Goblin::updateTarget() noexcept {
	if (canSeePlayer()) {
		targetPosition = player->position();
		aiState_ = AiState::ATTACKING;
		targetPriority = 1.;
	} else if (aiState_ == AiState::ATTACKING) {
		targetPosition = tryFollowStairs(targetPosition);
		aiState_ = AiState::SEEKING;
	} else if (aiState_ == AiState::SEEKING && position() == targetPosition) {
		targetPosition = randomNearbyTarget();
		targetPriority = 0.;
	} else
		targetPriority *= 0.9;
}

sf::Vector3i Goblin::randomNearbyTarget() noexcept {
	int minLevel = std::max(position().z - 1, 0);
	int maxLevel = std::min(position().z + 1, world().dungeon().size() - 1);
	int targetLevel = std::uniform_int_distribution{ minLevel, maxLevel }(randomEngine());
	return world().dungeon().randomPositionAt(targetLevel, randomEngine(), [this](sf::Vector3i pos, const Dungeon&) {
		return world().isFree(pos);
	});
}

sf::Vector3i Goblin::tryFollowStairs(sf::Vector3i position) noexcept {
	if (auto destination = world().dungeon().upStairs(position))
		return *destination;
	else if (auto destination = world().dungeon().downStairs(position))
		return *destination;
	return position;
}

void Goblin::travelToTarget() noexcept {
	sf::Vector3i nextStep_ = nextStep(world().dungeon(), position(), targetPosition);
	if (nextStep_.z == 0)
		tryMoveInDirection(getXY(nextStep_), false);
	else
		tryMove(nextStep_, false);
}

AiState Goblin::aiState() const noexcept {
	return aiState_;
}

void Goblin::spawnSingle(int level, std::shared_ptr<World> world, std::shared_ptr<Player> player_, RandomEngine& randomEngine) {
	sf::Vector3i position = world->dungeon().randomPositionAt(level, randomEngine, [world](sf::Vector3i pos, const Dungeon&) {
		return world->isFree(pos);
	});

	world->addActor(std::make_shared<Goblin>(position, world, std::move(player_), randomEngine));
}

void Goblin::spawnAll(std::shared_ptr<World> world, std::shared_ptr<Player> player_, RandomEngine& randomEngine) {
	for (int level = 0; level < world->dungeon().size(); ++level)
		for (int i = 0; i < std::uniform_int_distribution{ 5, 20 }(randomEngine); ++i)
			spawnSingle(level, world, player_, randomEngine);
}

bool Goblin::canSeePlayer() const noexcept {
	return uniformDistance(position(), player->position()) <= 7
		&& canSee(position(), player->position(), world().dungeon());
}

void Goblin::handleSound(sf::Vector3i soundPosition, double volume) noexcept {
	if (soundPosition.z != position().z)
		return;

	if (canSee(position(), soundPosition, world().dungeon()))
		return;

	auto [dx, dy, dz] = soundPosition - position();
	double distance = dx * dx + dy * dy;

	double priority = volume / distance;
	if (priority > targetPriority) {
		targetPosition = soundPosition;
		targetPriority = priority;
		aiState_ = AiState::SEEKING;
	}
}
