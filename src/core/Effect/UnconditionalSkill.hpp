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

#ifndef UNCONDITIONAL_SKILL_HPP_
#define UNCONDITIONAL_SKILL_HPP_

#include "Effect.hpp"

namespace core {
	/// @brief Always applies
	/// @details Type in skill file is "unconditional".
	/// Default skill type.
	class UnconditionalSkill : public Effect {
	public:
		UnconditionalSkill(double newRegenBonus, double newManaRegenBonus, double newDamageBonus, double newSpeedBonus,
			               double newAccuracyBonus, double newEvasionBonus, 
			               double newXpMul, double newHpBonus, double newManaBonus,
						   std::array<double, util::nEnumerators<DamageType>> defenceBonuses_,
			               const sf::Texture& icon_, std::string_view name_) :
			Effect{icon_, name_, true}, 
			hpBonus_{newHpBonus}, regenBonus_{newRegenBonus}, 
			manaBonus_{newManaBonus}, manaRegenBonus_{newManaRegenBonus},
			damageBonus_{newDamageBonus},
			accuracyBonus_{newAccuracyBonus}, evasionBonus_{newEvasionBonus},
			speedBonus_{newSpeedBonus}, xpMul_{newXpMul}, defenceBonuses{defenceBonuses_} {}

		double regenBonus() const final {
			return regenBonus_;
		}

		double manaRegenBonus() const final {
			return manaRegenBonus_;
		}

		double damageBonus(const Actor&) const final {
			return damageBonus_;
		}

		double speedBonus() const final {
			return speedBonus_;
		}

		double accuracyBonus() const final {
			return accuracyBonus_;
		}

		double evasionBonus() const final {
			return evasionBonus_;
		}

		double xpMul() const final {
			return xpMul_;
		}

		double hpBonus() const final {
			return hpBonus_;
		}

		double manaBonus() const final {
			return manaBonus_;
		}

		double defenceBonus(DamageType damageType) const final {
			return defenceBonuses[static_cast<size_t>(damageType)];
		}

		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<UnconditionalSkill>(*this);
		}
	private:
		double hpBonus_;
		double regenBonus_;
		double manaBonus_;
		double manaRegenBonus_;
		double damageBonus_;
		double accuracyBonus_;
		double evasionBonus_;
		double speedBonus_;
		double xpMul_;
		std::array<double, util::nEnumerators<DamageType>> defenceBonuses;
	};
}

#endif