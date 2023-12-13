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

#include "ConditionalBonus.hpp"

#include "../Actor.hpp"

#include "util/Exception.hpp"

namespace core {
	/// @brief Applies if Actor has low hp
	/// @detail Actor hp should be less than 50% of his max hp
	/// Type in skill file is "lowHp"
	class LowHpSkill : public ConditionalBonus {
	public:
		struct RequirementNotMet : public util::RuntimeError {
		public:
			using RuntimeError::RuntimeError;
		};

		LowHpSkill(Bonuses bonuses, const sf::Texture& icon, std::string_view name) :
				ConditionalBonus{bonuses, icon, name, true} {
			if (bonuses.hpBonus != 0)
				throw RequirementNotMet{"LowHpSkill can't change max hp"};
			if (bonuses.manaBonus != 0)
				throw RequirementNotMet{"LowHpSkill can't change max mana"};
		}

		bool shouldApply() const final {
			auto ownerPtr = owner_.lock();
			return ownerPtr->hp() < 0.5 * ownerPtr->maxHp();
		}

		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<LowHpSkill>(*this);
		}

		void owner(std::weak_ptr<Actor> newOwner) final {
			owner_ = std::move(newOwner);
		}
	private:
		std::weak_ptr<Actor> owner_;
	};
}

#endif