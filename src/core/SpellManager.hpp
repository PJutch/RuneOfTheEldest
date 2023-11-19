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

#ifndef SPELL_MANAGER_HPP_
#define SPELL_MANAGER_HPP_

#include "Spell.hpp"

#include "render/fwd.hpp"

#include "util/log.hpp"

#include <algorithm>
#include <string_view>
#include <vector>
#include <memory>

namespace core {
	/// Manages loading effects
	class SpellManager {
	public:
		SpellManager() = default;
		SpellManager(std::shared_ptr<render::AssetManager> assets,
			         util::LoggerFactory& loggerFactory) {
			
		}

		auto begin() {
			return spells.begin();
		}

		auto begin() const {
			return spells.begin();
		}

		auto cbegin() const {
			return spells.cbegin();
		}

		auto end() {
			return spells.end();
		}

		auto end() const {
			return spells.end();
		}

		auto cend() const {
			return spells.cend();
		}

		std::shared_ptr<Spell> findSpell(std::string_view spellName) const {
			auto iter = std::ranges::find(spells, spellName, [](const auto& spell) {
				return spell->name();
			});
			return iter == spells.end() ? nullptr : *iter;
		}
	private:
		std::vector<std::shared_ptr<Spell>> spells;
	};
}

#endif