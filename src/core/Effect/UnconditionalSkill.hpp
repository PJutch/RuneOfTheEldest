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

#include "ConditionalBonus.hpp"

#include "render/fwd.hpp"

#include <boost/describe.hpp>

#include <memory>

namespace core {
	/// @brief Always applies
	/// @details Type in skill file is "unconditional".
	/// Default skill type.
	class UnconditionalSkill : public ConditionalBonus {
	public:
		struct Data {
			StatBoosts boosts;
			std::string iconPath;
			std::string name;
		};

		UnconditionalSkill(Data data, std::string_view id, std::shared_ptr<render::AssetManager> assets) :
			ConditionalBonus{data.boosts, assets->texture(data.iconPath), id, data.name, true} {}

		bool shouldApply() const final {
			return true;
		}
		
		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<UnconditionalSkill>(*this);
		}
	};

	BOOST_DESCRIBE_STRUCT(UnconditionalSkill::Data, (), (boosts, iconPath, name))
}

#endif