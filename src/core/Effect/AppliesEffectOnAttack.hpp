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
			std::string iconPath;
			std::string name;
		};

		AppliesEffectOnAttack(Data data, std::string_view id_, std::shared_ptr<render::AssetManager> assets) :
			Effect{assets->texture(data.iconPath), id_, data.name, false}, appliedEffectName{data.applies} {}

		class UnknownEffect : public util::RuntimeError {
		public:
			UnknownEffect(std::string_view name, util::Stacktrace stacktrace = {}) :
				util::RuntimeError{std::format("Can't find effect \"{}\"", name), std::move(stacktrace)} {}
		};

		void init(const EffectManager& effects) final {
			appliedEffect = effects.findEffect(appliedEffectName);
			if (!appliedEffect)
				throw UnknownEffect{appliedEffectName};
		}

		void onAttack(Actor& actor) const final {
			actor.addEffect(appliedEffect->clone());
		}

		std::unique_ptr<Effect> clone() const final {
			return std::make_unique<AppliesEffectOnAttack>(*this);
		}
	private:
		std::string appliedEffectName;
		const Effect* appliedEffect = nullptr;
	};

	BOOST_DESCRIBE_STRUCT(AppliesEffectOnAttack::Data, (), (applies, iconPath, name))
}

#endif