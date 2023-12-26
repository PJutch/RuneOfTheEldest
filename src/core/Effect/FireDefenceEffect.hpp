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

#ifndef FIRE_DEFENCE_EFFECT_HPP_
#define FIRE_DEFENCE_EFFECT_HPP_

#include "Effect.hpp"

#include "../Actor.hpp"

namespace core {
	/// Modifies speed for some time
	class FireDefenceEffect : public Effect {
	public:
		FireDefenceEffect(double newDefenceBonus, double duration_, 
				const sf::Texture& icon_, std::string_view id_, std::string_view name_) :
			Effect{icon_, id_, name_, false}, defenceBonus_{newDefenceBonus}, duration{duration_} {}

		double defenceBonus(DamageType damageType) const final {
			if (damageType == DamageType::FIRE) {
				return defenceBonus_;
			} else {
				return 0;
			}
		}

		void update(double time) final {
			duration -= time;
		}

		virtual bool shouldBeRemoved() const {
			return duration <= 0;
		}

		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<FireDefenceEffect>(*this);
		}
	private:
		double defenceBonus_;
		double duration;
	};
}

#endif