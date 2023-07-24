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

#include "EnemyAi.hpp"

#include "Actor.hpp"

#include "util/pathfinding.hpp"
#include "util/raycast.hpp"
#include "util/geometry.hpp"
#include "util/assert.hpp"

namespace core {
	EnemyAi::EnemyAi(std::weak_ptr<Actor> newEnemy_) :
		enemy_{ std::move(newEnemy_) }, targetPosition{ enemy_.lock()->position() } {}

	bool EnemyAi::act() {
		updateTarget();
		travelToTarget();
		enemy_.lock()->endTurn();
		return true;
	}

	void EnemyAi::updateTarget() noexcept {
		auto enemy = enemy_.lock();
		if (canSeePlayer()) {
			setTarget(enemy->world().player().position(), 1.);
			return;
		}

		if (targetPriority < 0.001) {
			wandering = false;
			return;
		}

		if (targetPriority < 0.01 && !wandering)
			wander();
		
		if (enemy->position() == targetPosition) {
			if (checkStairs) {
				targetPosition = tryFollowStairs(targetPosition);
				checkStairs = false;
			} else
				wander();
		}
			
		targetPriority *= 0.9;
	}

	sf::Vector3i EnemyAi::randomNearbyTarget() noexcept {
		auto enemy = enemy_.lock();

		int minLevel = std::max(enemy->position().z - 1, 0);
		int maxLevel = std::min(enemy->position().z + 1, enemy->world().tiles().shape().z - 1);
		int targetLevel = std::uniform_int_distribution{ minLevel, maxLevel }(enemy->randomEngine());
		return enemy->world().randomPositionAt(targetLevel, &World::isFree);
	}

	sf::Vector3i EnemyAi::tryFollowStairs(sf::Vector3i position) noexcept {
		auto enemy = enemy_.lock();

		if (auto destination = enemy->world().upStairs(position))
			return *destination;
		else if (auto destination = enemy->world().downStairs(position))
			return *destination;
		return position;
	}

	void EnemyAi::travelToTarget() noexcept {
		auto enemy = enemy_.lock();

		wantsSwap(true);
		sf::Vector3i nextStep_ = util::nextStep(enemy->world(), enemy->position(), targetPosition);
		if (nextStep_.z == 0)
			enemy->tryMoveInDirection(util::getXY(nextStep_), false);
		else
			enemy->tryMove(nextStep_, false);
	}

	bool EnemyAi::canSeePlayer() const noexcept {
		auto enemy = enemy_.lock();
		return util::canSee(enemy->position(), enemy->world().player().position(), enemy->world());
	}

	void EnemyAi::handleSound(Sound sound) noexcept {
		auto enemy = enemy_.lock();

		if (util::canSee(enemy->position(), sound.position, enemy->world()))
			return; // Ignore sounds with known sources

		if (sound.type == Sound::Type::WALK && !sound.isSourceOnPlayerSide)
			return; // Ignore friend's WALK to prevent chasing each other on corners

		double priority = sound.volume(enemy->position());
		if (wandering) {
			if (priority > 0.01)
				setTarget(sound.position, priority);
		} else if (priority > targetPriority)
			setTarget(sound.position, priority);
	}

	AiState EnemyAi::aiState() const noexcept {
		if (canSeePlayer())
			return AiState::ATTACKING;
		else if (targetPriority < 0.001)
			return AiState::INACTIVE;
		else
			return AiState::SEEKING;
	}
}
