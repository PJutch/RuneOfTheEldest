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

#include "Potion.hpp"
#include "Equipment.hpp"

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
			        std::shared_ptr<SpellManager> spells, std::shared_ptr<EffectManager> effects, 
			        std::shared_ptr<XpManager> xpManager, std::shared_ptr<core::World> world,
					util::RandomEngine& randomEngine, util::LoggerFactory& loggerFactory);

		void load();

		std::unique_ptr<Item> newItem(std::string_view id);

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

		void parsePotionTextures(std::string_view data);
		std::string stringifyPotionTextures() const;

		void randomizeTextures();

		const sf::Texture* potionIcon(std::string_view id) {
			if (auto iter = std::ranges::find(potions, id, [](const auto& item) {
				return item.id;
			}); iter != potions.end()) {
				return potionTextures[iter - potions.begin()];
			} else {
				return nullptr;
			}
		}
	private:
		std::vector<Potion::Stats> potions;
		std::vector<const sf::Texture*> potionTextures;
		std::unordered_set<std::string> identifiedItems;
		std::vector<Equipment::Stats> equipment;

		std::shared_ptr<spdlog::logger> logger;

		std::shared_ptr<World> world;
		std::shared_ptr<SpellManager> spells;
		std::shared_ptr<EffectManager> effects;
		std::shared_ptr<XpManager> xpManager;
		std::shared_ptr<render::AssetManager> assets;
		util::RandomEngine* randomEngine;
	};
}

#endif