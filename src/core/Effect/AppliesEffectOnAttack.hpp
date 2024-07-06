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

#ifndef APPLIES_EFFECT_ON_ATTACK_HPP_
#define APPLIES_EFFECT_ON_ATTACK_HPP_

#include "Effect.hpp"

#include "../Actor.hpp"

#include <boost/describe.hpp>

namespace core {
	/// Lose hp with time. Type name is poison.
	class AppliesEffectOnAttack : public Effect {
	public:
		struct Data {
			std::string applies;
			const sf::Texture* icon;
			std::string name;
		};

		AppliesEffectOnAttack(Data data_, std::string_view newId) : data{data_}, id_{newId} {}

		class UnknownEffect : public util::RuntimeError {
		public:
			UnknownEffect(std::string_view name, util::Stacktrace stacktrace = {}) :
				util::RuntimeError{std::format("Can't find effect \"{}\"", name), std::move(stacktrace)} {}
		};

		void init(const EffectManager& effects) final {
			appliedEffect = effects.findEffect(data.applies);
			if (!appliedEffect)
				throw UnknownEffect{data.applies};
		}

		void onAttack(Actor& actor) const final {
			actor.addEffect(appliedEffect->clone());
		}

		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<AppliesEffectOnAttack>(*this);
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
		const Effect* appliedEffect = nullptr;
	};

	BOOST_DESCRIBE_STRUCT(AppliesEffectOnAttack::Data, (), (applies, icon, name))
}

#endif