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

#include "Actor.hpp"

#include "util/geometry.hpp"

namespace core {
	Actor::Actor(Stats stats_, sf::Vector3i newPosition,
		         std::shared_ptr<World> newWorld, std::shared_ptr<XpManager> xpManager_,
		         util::RandomEngine* newRandomEngine) :
		stats{stats_}, position_{ newPosition }, hp_{ stats.maxHp }, 
		world_{ std::move(newWorld) }, xpManager{ std::move(xpManager_) }, randomEngine_ {newRandomEngine} {}

	Actor::Actor(Stats stats_, 
		         std::shared_ptr<World> newWorld, std::shared_ptr<XpManager> xpManager, 
		         util::RandomEngine* newRandomEngine) :
		Actor{ stats_, {0, 0, 0}, std::move(newWorld), std::move(xpManager), newRandomEngine } {}

	void Actor::endTurn() noexcept {
		nextTurn_ += turnDelay();

		hp_ += regen() * turnDelay();
		hp_ = std::min(hp(), maxHp());
	}

	bool Actor::tryMoveTo(sf::Vector3i newPosition, bool forceSwap) {
		TROTE_ASSERT(world().tiles().isValidPosition(newPosition));

		if (newPosition == position())
			return false;

		if (!isPassable(world().tiles()[newPosition]))
			return false;

		auto other = world().actorAt(newPosition);
		if (!other) {
			position(newPosition);
			world().makeSound({ Sound::Type::WALK, controller().isOnPlayerSide(), position() });
			return true;
		}

		if (other->controller().isOnPlayerSide() != controller().isOnPlayerSide()) {
			other->beAttacked(damage(*other), stats.accuracy);
			world().makeSound({ Sound::Type::ATTACK, controller().isOnPlayerSide(), position()});
			return true;
		}

		if (!other->controller().wantsSwap() && !forceSwap)
			return false;

		sf::Vector3i oldPosition = position();
		position(other->position());
		other->position(oldPosition);

		controller().handleSwap();
		other->controller().handleSwap();
		world().makeSound({ Sound::Type::WALK, controller().isOnPlayerSide(), position() });
		return true;
	}

	bool Actor::canMoveToOrAttack(sf::Vector3i newPosition, bool forceSwap) const {
		TROTE_ASSERT(world().tiles().isValidPosition(newPosition));

		if (!isPassable(world().tiles()[newPosition]))
			return false;

		auto other = world().actorAt(newPosition);
		if (!other)
			return true;

		if (other->controller().isOnPlayerSide() != controller().isOnPlayerSide())
			return true;

		return forceSwap || other->controller().wantsSwap();
	}

	void Actor::tryMoveInDirection(sf::Vector2i direction, bool forceSwap) {
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

	double Actor::regen() {
		double res = stats.regen;
		for (const auto& skill : skills)
			res *= skill->regenMul();
		return res;
	}

	double Actor::damage(const Actor& target) {
		double res = stats.damage;
		for (const auto& skill : skills)
			res *= skill->damageMul(target);
		return res;
	}

	double Actor::turnDelay() {
		double speedMod = 1;
		for (const auto& skill : skills)
			speedMod += skill->speedMod();

		return stats.turnDelay / speedMod;
	}

	double Actor::xpMul() const {
		double res = 1;
		for (const auto& skill : skills)
			res *= skill->xpMul();
		return res;
	}

	void Actor::updateHp() {
		double newMul = 1;
		for (const auto& skill : skills)
			newMul *= skill->hpMul();

		hp_ *= newMul / hpMul;
		hpMul = newMul;
	}
}
