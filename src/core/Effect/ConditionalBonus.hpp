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

#ifndef CONDITIONAL_BONUS_HPP_
#define CONDITIONAL_BONUS_HPP_

#include "Effect.hpp"

#include "core/StatBoosts.hpp"
#include "core/Actor.hpp"

#include <array>

namespace core {
	/// Provides bonuses based on condition
	class ConditionalBonus : public Effect {
	public:
		ConditionalBonus(StatBoosts boosts_,
				const sf::Texture& icon_, std::string_view id_, std::string_view name_, bool isSkill) :
			Effect{icon_, id_, name_, isSkill}, boosts{boosts_} {}

		double regenBonus() const final {
			return shouldApply() ? boosts.regenBonus : 0;
		}

		double manaRegenBonus() const final {
			return shouldApply() ? boosts.manaRegenBonus : 0;
		}

		double damageBonus(const Actor&) const final {
			return shouldApply() ? boosts.damageBonus : 0;
		}

		double speedBonus() const final {
			return shouldApply() ? boosts.speedBonus : 0;
		}

		double accuracyBonus() const final {
			return shouldApply() ? boosts.accuracyBonus : 0;
		}

		double evasionBonus() const final {
			return shouldApply() ? boosts.evasionBonus : 0;
		}

		double xpMul() const final {
			return shouldApply() ? boosts.xpMul : 1;
		}

		double hpBonus() const final {
			return shouldApply() ? boosts.hpBonus : 0;
		}

		double manaBonus() const final {
			return shouldApply() ? boosts.manaBonus : 0;
		}

		double defenceBonus(DamageType damageType) const final {
			return shouldApply() ? boosts.defenceBonuses[damageType] : 0;
		}

		virtual bool shouldApply() const = 0;
	private:
		StatBoosts boosts;
	};
}

#endif