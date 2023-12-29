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

#ifndef ITEM_MANAGER_HPP_
#define ITEM_MANAGER_HPP_

#include "Item.hpp"

#include "Spell/Manager.hpp"

#include "render/fwd.hpp"

#include "util/log.hpp"

namespace core {
	/// Manages loading items
	class ItemManager {
	public:
		ItemManager() = default;
		ItemManager(std::shared_ptr<render::AssetManager> assets,
			        std::shared_ptr<SpellManager> spells, util::RandomEngine& randomEngine, 
					util::LoggerFactory& loggerFactory);

		auto begin() {
			return items.begin();
		}

		auto begin() const {
			return items.begin();
		}

		auto cbegin() const {
			return items.cbegin();
		}

		auto end() {
			return items.end();
		}

		auto end() const {
			return items.end();
		}

		auto cend() const {
			return items.cend();
		}

		const Item* findItem(std::string_view id) const {
			auto iter = std::ranges::find(items, id, [](const auto& item) {
				return item->id();
			});
			return iter == items.end() ? nullptr : iter->get();
		}
	private:
		std::vector<std::unique_ptr<Item>> items;
	};
}

#endif