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
			const sf::Texture* icon;
		};

		struct RequirementNotMet : public util::RuntimeError {
		public:
			using RuntimeError::RuntimeError;
		};

		TempBonus(Data data_, std::string_view newId) : data{data_}, id_{newId} {
			if (data.boosts.hp != 0)
				throw RequirementNotMet{"TempBonus can't change max hp"};
			if (data.boosts.mana != 0)
				throw RequirementNotMet{"TempBonus can't change max mana"};
		}

		bool shouldApply() const final {
			return true;
		}

		void update(double time) final {
			data.duration -= time;
		}

		bool shouldBeRemoved() const final {
			return data.duration <= 0;
		}

		[[nodiscard]] bool isCancelable() const final {
			return true;
		}

		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<TempBonus>(*this);
		}
		
		void parseData(std::string_view parsedData) final {
			util::KeyValueVisitor visitor;

			visitor.key("duration").unique().required().callback([&](std::string_view parsedData) {
				data.duration = util::parseReal(parsedData);
			});

			util::forEackInlineKeyValuePair(parsedData, visitor);
			visitor.validate();
		}

		[[nodiscard]] std::optional<std::string> stringify() const final {
			return std::format("{} duration {}", id(), data.duration);
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
			return false;
		}
	protected:
		[[nodiscard]] const StatBoosts& boosts() const final {
			return data.boosts;
		}
	private:
		Data data;
		std::string id_;
	};

	BOOST_DESCRIBE_STRUCT(TempBonus::Data, (), (duration, boosts, name, icon))
}

#endif