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

#include <unordered_set>

namespace core {
	/// Manages loading items
	class ItemManager : public std::enable_shared_from_this<ItemManager> {
	public:
		ItemManager() = default;
		ItemManager(std::shared_ptr<render::AssetManager> assets,
			        std::shared_ptr<SpellManager> spells, std::shared_ptr<core::World> world, 
					util::RandomEngine& randomEngine, util::LoggerFactory& loggerFactory);

		void load();

		const Item* findItem(std::string_view id) const {
			if (auto iter = std::ranges::find(scrolls, id, [](const auto& item) {
				return item->id();
			}); iter != scrolls.end()) {
				return iter->get();
			} else if (auto iter = std::ranges::find(potions, id, [](const auto& item) {
				return item->id();
			}); iter != potions.end()) {
				return iter->get();
			} else {
				return nullptr;
			}
		}

		Item* findItem(std::string_view id) {
			if (auto iter = std::ranges::find(scrolls, id, [](const auto& item) {
				return item->id();
			}); iter != scrolls.end()) {
				return iter->get();
			} else if (auto iter = std::ranges::find(potions, id, [](const auto& item) {
				return item->id();
			}); iter != potions.end()) {
				return iter->get();
			} else {
				return nullptr;
			}
		}

		void spawn();

		bool isIdentified(std::string_view itemId) {
			return identifiedItems.contains(std::string{itemId});
		}

		void identify(std::string_view itemId) {
			identifiedItems.insert(std::string{itemId});
		}

		void clearIdentifiedItems() {
			identifiedItems.clear();
		}

		void parseIdentifiedItems(std::string_view data);
		std::string stringifyIdentifiedItems() const;

		void parseItemTextures(std::string_view data);
		std::string stringifyItemTextures() const;

		void randomizeTextures();
	private:
		std::vector<std::shared_ptr<Item>> scrolls;
		std::vector<std::shared_ptr<Item>> potions;
		std::unordered_set<std::string> identifiedItems;

		std::shared_ptr<spdlog::logger> logger;

		std::shared_ptr<World> world;
		std::shared_ptr<SpellManager> spells;
		std::shared_ptr<render::AssetManager> assets;
		util::RandomEngine* randomEngine;
	};
}

#endif