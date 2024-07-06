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

#ifndef POISON_HPP_
#define POISON_HPP_

#include "Effect.hpp"

#include "../Actor.hpp"

#include "util/parseKeyValue.hpp"

#include <boost/describe.hpp>

namespace core {
	/// Lose hp with time. Type name is poison.
	class Poison : public Effect {
	public:
		struct Data {
			double damageOverTime;
			double duration;
			const sf::Texture* icon;
			std::string name;
		};

		Poison(Data data_, std::string_view newId) : data{data_}, id_{newId} {}

		void update(double time) final {
			owner_.lock()->beDamaged(time * data.damageOverTime, DamageType::POISON);
			data.duration -= time;
		}

		virtual bool shouldBeRemoved() const {
			return data.duration <= 0;
		}

		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<Poison>(*this);
		}

		void owner(std::weak_ptr<Actor> newOwner) final {
			owner_ = std::move(newOwner);
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
	private:
		Data data;
		std::string id_;

		std::weak_ptr<Actor> owner_;
	};

	BOOST_DESCRIBE_STRUCT(Poison::Data, (), (damageOverTime, duration, icon, name))
}

#endif