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
#include "util/reduce.hpp"

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

		for (const auto& effect : effects())
			effect->update(turnDelay());

		std::erase_if(effects_, [](const auto& effect) {
			return effect->shouldBeRemoved();
		});
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
			world().makeSound({Sound::Type::WALK, controller().isOnPlayerSide(), position()});
			return true;
		}

		if (other->controller().isOnPlayerSide() != controller().isOnPlayerSide()) {
			attack(*other);
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
		return stats.regen * util::reduce(effects_, 1., std::plus<>{}, [](const auto& effect) {
			return effect->regenBonus();
		});
	}

	double Actor::damage(const Actor& target) {
		return stats.damage * util::reduce(effects_, 1., std::plus<>{}, [&target](const auto& effect) {
			return effect->damageBonus(target);
		});
	}

	double Actor::turnDelay() {
		return stats.turnDelay / util::reduce(effects_, 1., std::plus<>{}, [](const auto& effect) {
			return effect->speedBonus();
		});
	}

	double Actor::xpMul() const {
		return util::reduce(effects_, 1., std::multiplies<>{}, [](const auto& effect) {
			return effect->xpMul();
		});
	}

	double Actor::evasion() {
		return util::reduce(effects_, stats.evasion, std::plus<>{}, [](const auto& effect) {
			return effect->evasionBonus();
		});
	}

	double Actor::accuracy() {
		return util::reduce(effects_, stats.accuracy, std::plus<>{}, [](const auto& effect) {
			return effect->accuracyBonus();
		});
	}

	double Actor::defence(DamageType damageType) {
		return util::reduce(effects_, stats.defences[static_cast<int>(damageType)], std::plus<>{}, [damageType](const auto& effect) {
			return effect->defenceBonus(damageType);
		});
	}

	void Actor::updateHp() {
		double newMul = util::reduce(effects_, 1., std::plus<>{}, [](const auto& effect) {
			return effect->hpBonus();
		});

		hp_ *= newMul / hpMul;
		hpMul = newMul;
	}

	void Actor::attack(Actor& other) {
		other.beAttacked(damage(other), stats.accuracy);
		for (const auto& effect : effects_)
			effect->onAttack(other);
		world().makeSound({Sound::Type::ATTACK, controller().isOnPlayerSide(), position()});
	}
}
