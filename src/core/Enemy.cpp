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

#include "Enemy.hpp"

#include "Player.hpp"

#include "util/pathfinding.hpp"
#include "util/raycast.hpp"
#include "util/geometry.hpp"
#include "util/assert.hpp"

namespace core {
	Enemy::Enemy(sf::Vector3i newPosition, double maxHp_, double regen_, const sf::Texture& texture, std::shared_ptr<World> world_,
		std::shared_ptr<Player> player_, util::RandomEngine& randomEngine_) :
		AliveActor{ maxHp_, regen_, texture, newPosition, std::move(world_), &randomEngine_ },
		player{ player_ }, targetPosition{ newPosition } {}

	bool Enemy::act() {
		updateTarget();
		travelToTarget();
		wait(1);
		return true;
	}

	void Enemy::updateTarget() noexcept {
		if (canSeePlayer()) {
			targetPosition = player->position();
			aiState_ = AiState::ATTACKING;
			targetPriority = 1.;
		}
		else if (aiState_ == AiState::ATTACKING) {
			targetPosition = tryFollowStairs(targetPosition);
			aiState_ = AiState::SEEKING;
		}
		else if (aiState_ == AiState::SEEKING && position() == targetPosition) {
			targetPosition = randomNearbyTarget();
			targetPriority = 0.01;
		}
		else
			targetPriority *= 0.9;
	}

	sf::Vector3i Enemy::randomNearbyTarget() noexcept {
		int minLevel = std::max(position().z - 1, 0);
		int maxLevel = std::min(position().z + 1, world().tiles().shape().z - 1);
		int targetLevel = std::uniform_int_distribution{ minLevel, maxLevel }(randomEngine());
		return world().randomPositionAt(targetLevel, &World::isFree);
	}

	sf::Vector3i Enemy::tryFollowStairs(sf::Vector3i position) noexcept {
		if (auto destination = world().upStairs(position))
			return *destination;
		else if (auto destination = world().downStairs(position))
			return *destination;
		return position;
	}

	void Enemy::travelToTarget() noexcept {
		wantsSwap_ = true;
		sf::Vector3i nextStep_ = util::nextStep(world(), position(), targetPosition);
		if (nextStep_.z == 0)
			tryMoveInDirection(util::getXY(nextStep_), false);
		else
			tryMove(nextStep_, false);
	}

	bool Enemy::canSeePlayer() const noexcept {
		return util::canSee(position(), player->position(), world());
	}

	namespace {
		double basicPriority(Sound sound) {
			switch (sound.type) {
			case Sound::Type::WALK:
				return sound.isSourceOnPlayerSide ? 0.1 : 0.0;
			case Sound::Type::ATTACK: return 0.5;
			default:
				TROTE_ASSERT(false, "All sound types sholud be handled");
			}
		}
	}

	void Enemy::handleSound(Sound sound) noexcept {
		if (sound.position.z != position().z)
			return;

		if (util::canSee(position(), sound.position, world()))
			return;

		auto [dx, dy, dz] = sound.position - position();
		double distance = dx * dx + dy * dy;

		double priority = basicPriority(sound) / distance;
		if (priority > targetPriority) {
			targetPosition = sound.position;
			targetPriority = priority;
			aiState_ = AiState::SEEKING;
		}
	}
}
