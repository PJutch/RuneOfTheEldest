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
			std::string iconPath;
			std::string name;
		};

		Poison(Data data, std::string_view id_, std::shared_ptr<render::AssetManager> assets) :
			Effect{assets->texture(data.iconPath), id_, data.name, false}, 
			damageOverTime{data.damageOverTime}, duration{data.duration} {}

		void update(double time) final {
			owner_.lock()->beDamaged(time * damageOverTime, DamageType::POISON);
			duration -= time;
		}

		virtual bool shouldBeRemoved() const {
			return duration <= 0;
		}

		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<Poison>(*this);
		}

		void owner(std::weak_ptr<Actor> newOwner) final {
			owner_ = std::move(newOwner);
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
		double damageOverTime;
		double duration;

		std::weak_ptr<Actor> owner_;
	};

	BOOST_DESCRIBE_STRUCT(Poison::Data, (), (damageOverTime, duration, iconPath, name))
}

#endif