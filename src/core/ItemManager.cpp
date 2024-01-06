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
#include "Potion.hpp"

#include "World.hpp"

#include "render/AssetManager.hpp"

#include "util/parse.hpp"
#include "util/filesystem.hpp"
#include "util/parseKeyValue.hpp"

namespace core {
	ItemManager::ItemManager(std::shared_ptr<render::AssetManager> assets,
							 std::shared_ptr<SpellManager> spells, std::shared_ptr<core::World> world_,
							 util::RandomEngine& randomEngine_, util::LoggerFactory& loggerFactory) : 
		logger{loggerFactory.create("items")}, world{std::move(world_)}, spells{std::move(spells)}, 
		assets{std::move(assets)}, randomEngine{&randomEngine_} {}

	void ItemManager::load() {
		logger->info("Loading...");

		// logger->info("Generating scrolls...");
		// for (const auto& spell : *spells | std::views::filter(&Spell::hasScroll)) {
		// 	items.push_back(std::make_shared<Scroll>(spell, shared_from_this(), assets, *randomEngine));
		// }

		logger->info("Loading potions...");
		std::filesystem::path basePath{"resources/descriptions/Potions/"};
		util::forEachFile(basePath, [&](std::ifstream& is, const std::filesystem::path& path) {
			std::string id = "potion." + util::toIdentifier(path, basePath);

			logger->info("Loading {} spec from {} ...", id, path.generic_string());

			util::KeyValueVisitor visitor;

			double hp = 0;
			visitor.key("hp").unique().callback([&](std::string_view data) {
				hp = util::parseReal(data);
			});

			std::string_view name;
			visitor.key("name").unique().callback([&](std::string_view data) {
				name = data;
			});

			util::forEackKeyValuePair(is, visitor);
			visitor.validate();

			items.push_back(std::make_shared<Potion>(hp, id, name, shared_from_this(), assets, *randomEngine));
		});

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

	void ItemManager::parseIdentifiedItems(std::string_view data) {
		for (std::string_view id : util::parseList(data)) {
			identify(util::strip(id));
		}
	}

	std::string ItemManager::stringifyIdentifiedItems() const {
		std::string result;
		for (std::string_view id : identifiedItems) {
			if (result.empty()) {
				result = id;
			} else {
				result += ", ";
				result += id;
			}
		}
		return result;
	}

	namespace {
		class UnknownItem : public util::RuntimeError {
		public:
			UnknownItem(std::string_view item, util::Stacktrace stacktrace = {}) :
				util::RuntimeError{std::format("Can't find \"{}\" item", item), std::move(stacktrace)} {}
		};
	}

	void ItemManager::parseItemTextures(std::string_view s) {
		util::forEackKeyValuePair(s, [&](std::string_view id, std::string_view data) {
			if (Item* item = findItem(id)) {
				item->parseTextureData(data);
			} else {
				throw UnknownItem{id};
			}
		});
	}

	std::string ItemManager::stringifyItemTextures() const {
		std::string result;
		for (const auto& item : items) {
			if (auto data = item->stringifyTextureData()) {
				result += std::format("{} {}", item->id(), *data);
			}
		}
		return result;
	}
}
