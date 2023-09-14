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

#ifndef APPLIES_EFFECT_ON_ATTACK_HPP_
#define APPLIES_EFFECT_ON_ATTACK_HPP_

#include "Effect.hpp"

#include "../Actor.hpp"

namespace core {
	/// Lose hp with time. Type name is poison.
	class AppliesEffectOnAttack : public Effect {
	public:
		AppliesEffectOnAttack(std::string_view appliedEffect_, const sf::Texture& icon_, std::string_view name_) :
			Effect{icon_, name_, false}, appliedEffectName{appliedEffect_} {}

		void init(const EffectManager& effects) final {
			appliedEffect = effects.findEffect(appliedEffectName);
		}

		void onAttack(Actor& actor) const final {
			actor.addEffect(appliedEffect->clone());
		}

		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<AppliesEffectOnAttack>(*this);
		}
	private:
		std::string appliedEffectName;
		const Effect* appliedEffect = nullptr;
	};
}

#endif