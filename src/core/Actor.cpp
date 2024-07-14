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

#include "util/Direction.hpp"
#include "util/reduce.hpp"

namespace core {
	Actor::Actor(Stats newStats, std::string newId, sf::Vector3i newPosition,
		         std::shared_ptr<World> newWorld, std::shared_ptr<XpManager> xpManager_,
				 std::shared_ptr<render::ParticleManager> particles_,
		         util::RandomEngine* newRandomEngine) :
			stats_{newStats}, id_{newId}, position_{newPosition}, hp_{newStats.maxHp}, mana_{newStats.maxMana},
			world_{ std::move(newWorld) }, xpManager{ std::move(xpManager_) },
			particles{particles_}, randomEngine_ {newRandomEngine} {
		equipment_[static_cast<int>(EquipmentSlot::RING)].resize(2);
	}

	Actor::Actor(Stats stats_, std::string newId,
		         std::shared_ptr<World> newWorld, std::shared_ptr<XpManager> xpManager, 
				 std::shared_ptr<render::ParticleManager> particles_,
		         util::RandomEngine* newRandomEngine) :
		Actor{ stats_, newId, {}, std::move(newWorld), std::move(xpManager), std::move(particles_), newRandomEngine } {}

	Actor::Actor(std::shared_ptr<World> newWorld, std::shared_ptr<XpManager> xpManager,
				 std::shared_ptr<render::ParticleManager> particles_,
				 util::RandomEngine* newRandomEngine) :
		Actor{{}, {}, std::move(newWorld), std::move(xpManager), std::move(particles_), newRandomEngine} {}

	void Actor::endTurn(std::shared_ptr<Spell> newCastedSpell) noexcept {
		if (castedSpell_) {
			if (castedSpell_ == newCastedSpell) {
				castedSpell_->continueCast();
			} else {
				castedSpell_->interrupt();
			}
		}
		castedSpell_ = std::move(newCastedSpell);

		nextTurn_ += turnDelay();

		hp_ += regen() * turnDelay();
		hp_ = std::min(hp(), maxHp());

		mana_ += manaRegen() * turnDelay();
		mana_ = std::min(mana(), maxMana());

		for (const auto& effect : effects())
			effect->update(turnDelay());

		std::erase_if(effects_, [](const auto& effect) {
			return effect->shouldBeRemoved();
		});

		std::erase_if(items_, [](const auto& item) {
			return item->shouldDestroy();
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

	void Actor::tryRunAwayInDirection(sf::Vector2i direction, bool forceSwap) {
		bool preferLeft = std::uniform_int_distribution<int>{0, 1}(randomEngine()) == 1;
		
		if (preferLeft) {
			for (auto current : util::NearestDirectionsLeftBiasRange{direction}) {
				TROTE_ASSERT(std::abs(current.x) <= 1 && std::abs(current.y) <= 1);
				if (tryMove(current, forceSwap)) return;
			}
		} else {
			for (auto current : util::NearestDirectionsRightBiasRange{direction}) {
				TROTE_ASSERT(std::abs(current.x) <= 1 && std::abs(current.y) <= 1);
				if (tryMove(current, forceSwap)) return;
			}
		}
	}

	void Actor::tryRunAwayFromPlayer(bool forceSwap) {
		sf::Vector3i playerPos = world().player().position();
		tryRunAwayInDirection(util::directionOf(util::getXY(position()) - util::getXY(playerPos)), forceSwap);
	}

	double Actor::regen() {
		return stats().regen * reduceStatBoosters(1., std::plus<>{}, &StatBooster::regenBonus);
	}

	double Actor::manaRegen() {
		return stats().manaRegen * reduceStatBoosters(1., std::plus<>{}, &StatBooster::manaRegenBonus);
	}

	double Actor::damage(const Actor& target) {
		return stats().damage * reduceStatBoosters(1., std::plus<>{}, [&](const StatBooster& booster) {
			return booster.damageBonus(target);
		});
	}

	double Actor::turnDelay() {
		double speed = reduceStatBoosters(0., std::plus<>{}, & StatBooster::speedBonus);
		return stats().turnDelay * (speed >= 0 ? 1 / (speed + 1) : 1 - speed);
	}

	double Actor::xpMul() const {
		return reduceStatBoosters(1., std::multiplies<>{}, &StatBooster::xpMul);
	}

	double Actor::evasion() {
		return reduceStatBoosters(stats().evasion, std::plus<>{}, &StatBooster::evasionBonus);
	}

	double Actor::accuracy() {
		return reduceStatBoosters(stats().accuracy, std::plus<>{}, &StatBooster::accuracyBonus);
	}

	double Actor::defence(DamageType damageType) {
		return reduceStatBoosters(stats().defences[damageType], std::plus<>{}, [damageType](const auto& booster) {
			return booster.defenceBonus(damageType);
		});
	}

	void Actor::updateHp() {
		double newMHpul = reduceStatBoosters(1., std::plus<>{}, &StatBooster::hpBonus);

		hp_ *= newMHpul / hpMul;
		hpMul = newMHpul;

		double newManaMul = reduceStatBoosters(1., std::plus<>{}, &StatBooster::manaBonus);

		mana_ *= newManaMul / manaMul;
		manaMul = newManaMul;
	}

	void Actor::attack(Actor& other) {
		other.beAttacked(damage(other), stats().accuracy, DamageType::PHYSICAL);
		for (const auto& effect : effects_)
			effect->onAttack(other);
		world().makeSound({Sound::Type::ATTACK, controller().isOnPlayerSide(), position()});
	}

	double Actor::recievedDamageMul(DamageType damageType) {
		double defence_ = defence(damageType);
		if (defence_ >= 0) {
			return 1 / (defence_ + 1);
		} else {
			return 1 - defence_;
		}
	}

	void Actor::beDamaged(double damage, DamageType type) {
		if (!isAlive())
			return;

		hp_ -= damage * recievedDamageMul(type);
		if (!isAlive())
			xpManager->addXp(stats().xp);
	}
}
