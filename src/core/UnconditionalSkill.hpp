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
		UnconditionalSkill(double newRegenMul, double newDamageMul, double newTurnDelayMul, double newXpMul,
			               const sf::Texture& icon_, std::string_view name_) :
			regenMul_{newRegenMul}, damageMul_{newDamageMul}, turnDelayMul_{newTurnDelayMul}, xpMul_{newXpMul}, Skill {icon_, name_} {}

		double regenMul() const final {
			return regenMul_;
		}

		double damageMul() const final {
			return damageMul_;
		}

		double turnDelayMul() const final {
			return turnDelayMul_;
		}

		double xpMul() const final {
			return xpMul_;
		}

		std::unique_ptr<Skill> clone() const override {
			return std::make_unique<UnconditionalSkill>(*this);
		}
	private:
		double regenMul_;
		double damageMul_;
		double turnDelayMul_;
		double xpMul_;
	};
}

#endif