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

#ifndef LOW_HP_SKILL_HPP_
#define LOW_HP_SKILL_HPP_

#include "Effect.hpp"

#include "../Actor.hpp"

namespace core {
	/// @brief Applies if Actor has low hp
	/// @detail Actor hp should be less than 50% of his max hp
	/// Type in skill file is "lowHp"
	class LowHpSkill : public Effect {
	public:
		LowHpSkill(double newRegenMul, double newDamageMul, double newSpeedBonus, double newXpMul, 
			       double newAccuracyBonus, double newEvasionBonus,
			       const sf::Texture& icon_, std::string_view name_) :
			Effect{icon_, name_, true}, regenMul_{newRegenMul}, damageMul_{newDamageMul},
			accuracyBonus_{newAccuracyBonus}, evasionBonus_{newEvasionBonus},
			speedBonus_{newSpeedBonus}, xpMul_{newXpMul} {}

		double regenMul() const final {
			return shouldApply() ? regenMul_ : 1;
		}

		double damageMul(const Actor&) const final {
			return shouldApply() ? damageMul_ : 1;
		}

		double speedBonus() const final {
			return shouldApply() ? speedBonus_ : 0;
		}

		double xpMul() const final {
			return shouldApply() ? xpMul_ : 1;
		}

		double accuracyBonus() const final {
			return shouldApply() ? accuracyBonus_ : 0;
		}

		double evasionBonus() const final {
			return shouldApply() ? evasionBonus_ : 0;
		}

		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<LowHpSkill>(*this);
		}

		void owner(std::weak_ptr<Actor> newOwner) final {
			owner_ = std::move(newOwner);
		}
	private:
		double regenMul_;
		double damageMul_;
		double accuracyBonus_;
		double evasionBonus_;
		double speedBonus_;
		double xpMul_;

		std::weak_ptr<Actor> owner_;

		bool shouldApply() const {
			auto ownerPtr = owner_.lock();
			return ownerPtr->hp() < 0.5 * ownerPtr->maxHp();
		}
	};
}

#endif