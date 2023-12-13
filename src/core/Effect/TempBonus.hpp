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

#ifndef TEMP_BONUS_HPP_
#define TEMP_BONUS_HPP_

#include "ConditionalBonus.hpp"

#include "../Actor.hpp"

namespace core {
	/// Modifies speed for some time
	class TempBonus : public ConditionalBonus {
	public:
		struct RequirementNotMet : public util::RuntimeError {
		public:
			using RuntimeError::RuntimeError;
		};

		TempBonus(Bonuses bonuses, double duration_, const sf::Texture& icon, std::string_view name) :
				ConditionalBonus{bonuses, icon, name, false}, duration{duration_} {
			if (bonuses.hpBonus != 0)
				throw RequirementNotMet{"TempBonus can't change max hp"};
			if (bonuses.manaBonus != 0)
				throw RequirementNotMet{"TempBonus can't change max mana"};
		}

		bool shouldApply() const final {
			return true;
		}

		void update(double time) final {
			duration -= time;
		}

		bool shouldBeRemoved() const final {
			return duration <= 0;
		}

		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<TempBonus>(*this);
		}
	private:
		double duration;
	};
}

#endif