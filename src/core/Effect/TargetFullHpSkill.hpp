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

#ifndef TARGET_FULL_HP_SKILL_HPP_
#define TARGET_FULL_HP_SKILL_HPP_

#include "Effect.hpp"

#include "../Actor.hpp"

namespace core {
	/// @brief Applies if attacked Actor has full hp
	/// @details Type in skill file is "targetFullHp"
	class TargetFullHpSkill : public Effect {
	public:
		struct Data {
			double damage;
			const sf::Texture* icon;
			std::string name; 
		};

		TargetFullHpSkill(Data data_, std::string_view newId) : data{data_}, id_{newId} {}

		double damageBonus(const Actor& target) const final {
			return target.hp() == target.maxHp() ? data.damage : 0;
		}

		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<TargetFullHpSkill>(*this);
		} 
		
		[[nodiscard]] const sf::Texture& icon() const final {
			return *data.icon;
		}

		[[nodiscard]] const std::string& id() const final {
			return id_;
		}

		[[nodiscard]] const std::string& name() const final {
			return data.name;
		}

		[[nodiscard]] bool isSkill() const final {
			return true;
		}
	private:
		Data data;
		std::string id_;
	};

	BOOST_DESCRIBE_STRUCT(TargetFullHpSkill::Data, (), (damage, icon, name))
}

#endif