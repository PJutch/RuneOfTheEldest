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

#include "ItemManager.hpp"

#include "Scroll.hpp"

#include "World.hpp"

#include "render/AssetManager.hpp"

namespace core {
	ItemManager::ItemManager(std::shared_ptr<render::AssetManager> assets,
							 std::shared_ptr<SpellManager> spells, std::shared_ptr<core::World> world_,
							 util::RandomEngine& randomEngine_, util::LoggerFactory& loggerFactory) : 
			logger{loggerFactory.create("items")}, world {std::move(world_)}, randomEngine{&randomEngine_} {
		logger->info("Loading...");

		logger->info("Generating scrolls...");
		for (const auto& spell : *spells | std::views::filter(&Spell::hasScroll)) {
			items.push_back(std::make_unique<Scroll>(spell, assets->scrollTexture(spell->icon()), spells, *randomEngine));
		}

		logger->info("Loaded");
	}

	void ItemManager::spawn() {
		logger->info("Spawning...");
		for (int z = 0; z < world->tiles().shape().z; ++z) {
			for (int i = 0; i < std::uniform_int_distribution{1, 4}(*randomEngine); ++i) {
				if (auto pos = world->randomPositionAt(z, 1000, &World::isFree)) {
					auto iitem = std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(items) - 1}(*randomEngine);
					world->addItem(core::Position<int>{*pos}, items[iitem]->clone());
				}
			}
		}
		logger->info("Spawned");
	}
}
