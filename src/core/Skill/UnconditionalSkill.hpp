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

#include "Skill.hpp"

namespace core {
	class UnconditionalSkill : public Skill {
	public:
		UnconditionalSkill(double newRegenMul, double newDamageMul, double newSpeedBonus, double newAccuracyBonus, double newEvasionBonus, 
			               double newXpMul, double hpMul,
			               const sf::Texture& icon_, std::string_view name_) :
			Skill{icon_, name_}, regenMul_{newRegenMul}, damageMul_{newDamageMul}, 
			accuracyBonus_{newSpeedBonus}, evasionBonus_{newEvasionBonus},
			speedBonus_{newSpeedBonus}, xpMul_{newXpMul}, hpMul_{hpMul} {}

		double regenMul() const final {
			return regenMul_;
		}

		double damageMul(const Actor&) const final {
			return damageMul_;
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

		double hpMul() const final {
			return hpMul_;
		}

		std::unique_ptr<Skill> clone() const final {
			return std::make_unique<UnconditionalSkill>(*this);
		}
	private:
		double hpMul_;
		double regenMul_;
		double damageMul_;
		double accuracyBonus_;
		double evasionBonus_;
		double speedBonus_;
		double xpMul_;
	};
}

#endif