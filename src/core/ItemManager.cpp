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
							 std::shared_ptr<SpellManager> spells_, std::shared_ptr<EffectManager> effects_, 
		                     std::shared_ptr<XpManager> xpManager_, std::shared_ptr<core::World> world_,
							 util::RandomEngine& randomEngine_, util::LoggerFactory& loggerFactory) : 
		logger{loggerFactory.create("items")}, world{std::move(world_)}, 
		spells{std::move(spells_)}, effects{std::move(effects_)}, xpManager{std::move(xpManager_)},
		assets{std::move(assets)}, randomEngine{&randomEngine_} {}

	namespace {
		class UnknownEffect : public util::RuntimeError {
		public:
			UnknownEffect(std::string_view name, util::Stacktrace stacktrace = {}) :
				util::RuntimeError{std::format("Unknow effect \"{}\"", name), std::move(stacktrace)} {}
		};
	}

	void ItemManager::load() {
		logger->info("Loading...");

		logger->info("Loading potions...");
		std::filesystem::path basePath{"resources/descriptions/Potions/"};
		util::forEachFile(basePath, [&](std::ifstream& is, const std::filesystem::path& path) {
			std::string id = "potion." + util::toIdentifier(path, basePath);

			logger->info("Loading {} spec from {} ...", id, path.generic_string());

			util::KeyValueVisitor visitor;
			Potion::Stats stats{.id = id};

			visitor.key("hp").unique().callback([&](std::string_view data) {
				stats.hp = util::parseReal(data);
			});

			visitor.key("mana").unique().callback([&](std::string_view data) {
				stats.mana = util::parseReal(data);
			});

			visitor.key("xp").unique().callback([&](std::string_view data) {
				stats.xp = util::parseReal(data);
			});

			visitor.key("effect").unique().callback([&](std::string_view data) {
				if (auto effect_ = effects->findEffect(data)) {
					stats.effect = effect_;
				} else {
					throw UnknownEffect{data};
				}
			});

			visitor.key("cancelEffects").unique().callback([&](std::string_view data) {
				stats.cancelEffects = util::parseBool(data);
			});

			visitor.key("name").unique().required().callback([&](std::string_view data) {
				stats.name = data;
			});

			visitor.key("label").unique().required().callback([&](std::string_view data) {
				stats.label = &assets->texture(data);
			});

			util::forEackKeyValuePair(is, visitor);
			visitor.validate();

			potions.push_back(stats);
		});

		logger->info("Loading potion textures...");
		util::forEachFile("resources/textures/Potions", [&](const std::filesystem::path& path) {
			potionTextures.push_back(&assets->texture(path));
		});
		randomizeTextures();

		logger->info("Loaded");
	}

	std::unique_ptr<Item> ItemManager::newItem(std::string_view id) {
		if (auto spellId = util::parsePrefixed(id, "scroll.")) {
			auto scrollableSpells = *spells | std::views::filter(&Spell::hasScroll);
			if (auto iter = std::ranges::find(scrollableSpells, *spellId, [](const auto& spell) {
				return spell->id();
			}); iter != scrollableSpells.end()) {
				return std::make_unique<Scroll>(*iter, shared_from_this(), assets, *randomEngine);
			}
		}

		if (auto iter = std::ranges::find(potions, id, [](const auto& item) {
			return item.id;
		}); iter != potions.end()) {
			return std::make_unique<Potion>(*iter, *potionTextures[iter - potions.begin()], shared_from_this(), xpManager, assets, *randomEngine);
		} else {
			return nullptr;
		}
	}

	void ItemManager::spawn() {
		std::vector<std::shared_ptr<Spell>> scrollableSpells;
		std::ranges::copy_if(*spells, std::back_inserter(scrollableSpells), &Spell::hasScroll);

		logger->info("Spawning...");
		for (int z = 0; z < world->tiles().shape().z; ++z) {
			for (int i = 0; i < std::uniform_int_distribution{3, 10}(*randomEngine); ++i) {
				if (auto pos = world->randomPositionAt(z, 1000, &World::isFree)) {
					if (std::uniform_real_distribution{}(*randomEngine) < 0.5) {
						auto ispell = std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(scrollableSpells) - 1}(*randomEngine);
						world->addItem(core::Position<int>{*pos}, std::make_unique<Scroll>(scrollableSpells[ispell], shared_from_this(), assets, *randomEngine));
					} else {
						auto iitem = std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(potions) - 1}(*randomEngine);
						auto item = std::make_unique<Potion>(potions[iitem], *potionTextures[iitem], shared_from_this(), xpManager, assets, *randomEngine);
						world->addItem(core::Position<int>{*pos}, std::move(item));
					}
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

	void ItemManager::parsePotionTextures(std::string_view s) {
		util::forEackKeyValuePair(s, [&](std::string_view id, std::string_view data) {
			int iitem = std::ranges::find(potions, id, [](const auto& item) {
				return item.id;
			}) - potions.begin();

			if (iitem == std::ssize(potions)) {
				throw UnknownItem{id};
			}

			potionTextures[iitem] = &assets->parse(data);
		});
	}

	std::string ItemManager::stringifyPotionTextures() const {
		std::string result;
		for (int i = 0; i < std::ssize(potions); ++i) {
			result += std::format("{} {}\n", potions[i].id, assets->stringify(*potionTextures[i]));
		}
		return result;
	}

	void ItemManager::randomizeTextures() {
		std::ranges::shuffle(potionTextures, *randomEngine);
	}
}
