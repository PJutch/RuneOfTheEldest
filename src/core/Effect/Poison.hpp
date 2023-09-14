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

namespace core {
	/// Lose hp with time. Type name is poison.
	class Poison : public Effect {
	public:
		Poison(double damageOverTime_, double duration_, const sf::Texture& icon_, std::string_view name_) :
			Effect{icon_, name_, false}, damageOverTime{damageOverTime_}, duration{duration_} {}

		void update(double time) final {
			owner_.lock()->beDamaged(time * damageOverTime);
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
	private:
		double damageOverTime;
		double duration;

		std::weak_ptr<Actor> owner_;
	};
}

#endif