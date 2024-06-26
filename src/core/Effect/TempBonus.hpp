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

#include <boost/describe.hpp>

namespace core {
	/// Modifies speed for some time
	class TempBonus : public ConditionalBonus {
	public:
		struct Data {
			double duration;
			StatBoosts boosts;
			std::string name;
			std::string iconPath;
		};

		struct RequirementNotMet : public util::RuntimeError {
		public:
			using RuntimeError::RuntimeError;
		};

		TempBonus(Data data, std::string_view id, std::shared_ptr<render::AssetManager> assets) :
				ConditionalBonus{data.boosts, assets->texture(data.iconPath), id, data.name, false}, duration{data.duration} {
			if (data.boosts.hp != 0)
				throw RequirementNotMet{"TempBonus can't change max hp"};
			if (data.boosts.mana != 0)
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

		[[nodiscard]] bool isCancelable() const final {
			return true;
		}

		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<TempBonus>(*this);
		}
		
		void parseData(std::string_view data) final {
			util::KeyValueVisitor visitor;

			visitor.key("duration").unique().required().callback([&](std::string_view data) {
				duration = util::parseReal(data);
			});

			util::forEackInlineKeyValuePair(data, visitor);
			visitor.validate();
		}

		[[nodiscard]] std::optional<std::string> stringify() const final {
			return std::format("{} duration {}", id(), duration);
		}
	private:
		double duration;
	};

	BOOST_DESCRIBE_STRUCT(TempBonus::Data, (), (duration, boosts, name, iconPath))
}

#endif