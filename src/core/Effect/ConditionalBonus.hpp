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
		ConditionalBonus() {}

		double regenBonus() const final {
			return shouldApply() ? boosts().regen : 0;
		}

		double manaRegenBonus() const final {
			return shouldApply() ? boosts().manaRegen : 0;
		}

		double damageBonus(const Actor&) const final {
			return shouldApply() ? boosts().damage : 0;
		}

		double speedBonus() const final {
			return shouldApply() ? boosts().speed : 0;
		}

		double accuracyBonus() const final {
			return shouldApply() ? boosts().accuracy : 0;
		}

		double evasionBonus() const final {
			return shouldApply() ? boosts().evasion : 0;
		}

		double xpMul() const final {
			return shouldApply() ? boosts().xpMul : 1;
		}

		double hpBonus() const final {
			return shouldApply() ? boosts().hp : 0;
		}

		double manaBonus() const final {
			return shouldApply() ? boosts().mana : 0;
		}

		double defenceBonus(DamageType damageType) const final {
			return shouldApply() ? boosts().defences[damageType] : 0;
		}

		virtual bool shouldApply() const = 0;
	protected:
		[[nodiscard]] virtual const StatBoosts& boosts() const = 0;
	};
}

#endif