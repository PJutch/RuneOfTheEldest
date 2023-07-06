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

#include "AliveActor.hpp"

#include "util/geometry.hpp"

namespace core {
	AliveActor::AliveActor(double newMaxHp, double regen_, const sf::Texture& texture, sf::Vector3i newPosition, 
		                   std::shared_ptr<World> newWorld, util::RandomEngine* newRandomEngine) :
		position_{ newPosition }, hp_{ newMaxHp }, maxHp_{ newMaxHp }, regen{ regen_ }, texture_{&texture},
		world_{ std::move(newWorld) }, randomEngine_{ newRandomEngine } {}

	AliveActor::AliveActor(double newMaxHp, double regen_, const sf::Texture& texture, 
		                   std::shared_ptr<World> newWorld, util::RandomEngine* newRandomEngine) :
		AliveActor{ newMaxHp, regen_, texture, {0, 0, 0}, std::move(newWorld), newRandomEngine } {}

	void AliveActor::wait(int time) noexcept {
		nextTurn_ += time;

		hp_ += regen * time;
		hp(std::min(hp(), maxHp()));
	}

	bool AliveActor::tryMoveTo(sf::Vector3i newPosition, bool forceSwap) {
		if (newPosition == position())
			return false;

		if (!isPassable(world().tiles()[newPosition]))
			return false;

		auto other = world().actorAt(newPosition);
		if (!other) {
			position(newPosition);
			moveSucceed();
			return true;
		}

		if (other->isOnPlayerSide() != isOnPlayerSide()) {
			attack(*other);
			return true;
		}

		if (!other->wantsSwap() && !forceSwap)
			return false;

		sf::Vector3i oldPosition = position();
		position(other->position());
		other->position(oldPosition);

		handleSwap();
		other->handleSwap();
		moveSucceed();
		return true;
	}

	bool AliveActor::canMoveToOrAttack(sf::Vector3i newPosition, bool forceSwap) const {
		if (!isPassable(world().tiles()[newPosition]))
			return false;

		auto other = world().actorAt(newPosition);
		if (!other)
			return true;

		if (other->isOnPlayerSide() != isOnPlayerSide())
			return true;

		return forceSwap || other->wantsSwap();
	}

	void AliveActor::tryMoveInDirection(sf::Vector2i direction, bool forceSwap) {
		if (tryMove(direction, forceSwap))
			return;

		bool preferLeft = std::uniform_int_distribution<int>{ 0, 1 }(randomEngine()) == 1;
		if (preferLeft) {
			if (tryMove(util::turnDirection45Left (direction), forceSwap)) return;
			if (tryMove(util::turnDirection45Right(direction), forceSwap)) return;
		} else {
			if (tryMove(util::turnDirection45Right(direction), forceSwap)) return;
			if (tryMove(util::turnDirection45Left (direction), forceSwap)) return;
		}
	}
}
