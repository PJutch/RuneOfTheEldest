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

#ifndef EFFECT_MANAGER_HPP_
#define EFFECT_MANAGER_HPP_

#include "Effect/Effect.hpp"
#include "Effect/ConditionalBonus.hpp"

#include "render/fwd.hpp"

#include "util/log.hpp"

namespace core {
	/// Manages loading effects
	class EffectManager {
	public:
		EffectManager() = default;
		EffectManager(std::shared_ptr<render::AssetManager> assets,
			          util::LoggerFactory& loggerFactory);

		auto begin() {
			return effects.begin();
		}

		auto begin() const {
			return effects.begin();
		}

		auto cbegin() const {
			return effects.cbegin();
		}

		auto end() {
			return effects.end();
		}

		auto end() const {
			return effects.end();
		}

		auto cend() const {
			return effects.cend();
		}

		const Effect* findEffect(std::string_view id) const {
			auto iter = std::ranges::find(effects, id, [](const auto& effect) {
				return effect->id();
			});
			return iter == effects.end() ? nullptr : iter->get();
		}
	private:
		std::vector<std::unique_ptr<Effect>> effects;
	};
}

#endif