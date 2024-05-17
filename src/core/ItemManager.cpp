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

		class UnknownEquipmentSlotError : public util::RuntimeError {
		public:
			UnknownEquipmentSlotError(std::string_view equipmentSlot, util::Stacktrace currentStacktrace = {}) noexcept :
				RuntimeError{std::format("Unknown equipment slot \"{}\"", equipmentSlot),
				std::move(currentStacktrace)} {}
		};

		EquipmentSlot getEquipmentSlot(const std::string& name) {
			if (auto v = util::getOptional(equipmentSlotByName, name))
				return *v;
			else
				throw UnknownEquipmentSlotError{name};
		}
	}

	void ItemManager::load() {
		logger->info("Loading...");

		logger->info("Loading potions...");
		std::filesystem::path potionPath{"resources/descriptions/Potions/"};
		util::forEachFile(potionPath, [&](std::ifstream& is, const std::filesystem::path& path) {
			std::string id = "potion." + util::toIdentifier(path, potionPath);

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

		logger->info("Loading equipment...");
		std::filesystem::path equipmentPath{"resources/descriptions/Equipment/"};
		util::forEachFile(equipmentPath, [&](std::ifstream& is, const std::filesystem::path& path) {
			Equipment::Stats stats;
			stats.id = "equipment." + util::toIdentifier(path, equipmentPath);

			logger->info("Loading {} spec from {} ...", stats.id, path.generic_string());

			auto params = util::parseMapping(is);

			stats.slot = getEquipmentSlot(util::getAndEraseRequired(params, "slot"));
			stats.name = util::getAndEraseRequired(params, "name");
			stats.boosts = loadBonuses(params);

			equipment[stats.slot].push_back(stats);
		});

		logger->info("Loading potion textures...");
		util::forEachFile("resources/textures/Potions", [&](const std::filesystem::path& path) {
			potionTextures.push_back(&assets->texture(path));
		});

		logger->info("Loading equipment textures...");
		const std::filesystem::path equipmentTexturesPath = "resources/textures/Equipment";
		boost::mp11::mp_for_each<boost::describe::describe_enumerators<EquipmentSlot>>([&](auto D) {
			util::forEachFile(equipmentTexturesPath / util::toTitle(D.name), [&](const std::filesystem::path& path) {
				equipmentTextures[static_cast<int>(D.value)].push_back(&assets->texture(path));
			});
		});

		randomizeTextures();

		logger->info("Loaded");
	}

	std::unique_ptr<Item> ItemManager::newItem(std::string_view id) {
		if (auto spellId = util::parsePrefixed(id, "scroll.")) {
			if (auto spell = spells->findSpell(*spellId); spell && spell->hasScroll()) {
				return std::make_unique<Scroll>(spell->clone(), shared_from_this(), assets, *randomEngine);
			}
		}

		if (auto iter = std::ranges::find(potions, id, [](const auto& item) {
			return item.id;
		}); iter != potions.end()) {
			return std::make_unique<Potion>(*iter, *potionTextures[iter - potions.begin()], shared_from_this(), xpManager, assets, *randomEngine);
		} else {
			std::unique_ptr<Item> res = nullptr;
			boost::mp11::mp_for_each<boost::describe::describe_enumerators<EquipmentSlot>>([&](auto D) {
				auto& slotEquipment = equipment[static_cast<int>(D.value)];
				if (auto iter = std::ranges::find(slotEquipment, id, [](const auto& item) {
					return item.id;
				}); iter != slotEquipment.end()) {
					const sf::Texture* texture = equipmentTextures[iter->slot][iter - slotEquipment.begin()];
					res = std::make_unique<Equipment>(*iter, texture, shared_from_this(), xpManager, assets, *randomEngine);
				}
			});
			return res;
		}
	}

	void ItemManager::spawn() {
		std::vector<std::shared_ptr<Spell>> scrollableSpells;
		std::ranges::copy_if(*spells, std::back_inserter(scrollableSpells), &Spell::hasScroll);

		logger->info("Spawning...");
		for (int z = 0; z < world->tiles().shape().z; ++z) {
			for (int i = 0; i < std::uniform_int_distribution{3, 10}(*randomEngine); ++i) {
				if (auto pos = world->randomPositionAt(z, 1000, &World::isFree)) {
					switch (std::uniform_int_distribution{0, 2}(*randomEngine)) {
					case 0: {
						auto ispell = std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(scrollableSpells) - 1}(*randomEngine);
						world->addItem(core::Position<int>{*pos}, std::make_unique<Scroll>(scrollableSpells[ispell], shared_from_this(), assets, *randomEngine));
						break;
					}
					case 1: {
						auto iitem = std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(potions) - 1}(*randomEngine);
						auto item = std::make_unique<Potion>(potions[iitem], *potionTextures[iitem], shared_from_this(), xpManager, assets, *randomEngine);
						world->addItem(core::Position<int>{*pos}, std::move(item));
						break;
					}
					case 2: {
						auto islot = std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(equipment) - 1}(*randomEngine);

						auto iitem = std::uniform_int_distribution<ptrdiff_t>{0, nEquipment(static_cast<EquipmentSlot>(islot)) - 1}(*randomEngine);

						auto item = std::make_unique<Equipment>(equipment[islot][iitem], equipmentTextures[islot][iitem], 
							                                    shared_from_this(), xpManager, assets, *randomEngine);
						world->addItem(core::Position<int>{*pos}, std::move(item));
						break;
					}
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

	void ItemManager::parseTextures(std::string_view s) {
		util::forEackKeyValuePair(s, [&](std::string_view id, std::string_view data) {
			if (auto iter = std::ranges::find(potions, id, [](const auto& item) {
				return item.id;
			}); iter != potions.end()) {
				potionTextures[iter - potions.begin()] = &assets->parse(data);
			} else {
				bool found = false;
				boost::mp11::mp_for_each<boost::describe::describe_enumerators<EquipmentSlot>>([&](auto D) {
					auto& slotEquipment = equipment[static_cast<int>(D.value)];
					if (auto iter = std::ranges::find(slotEquipment, id, [](const auto& item) {
						return item.id;
					}); iter != slotEquipment.end()) {
						equipmentTextures[iter->slot][iter - slotEquipment.begin()] = &assets->parse(data);
						found = true;
					}
				});

				if (!found) {
					throw UnknownItem{id};
				}
			}	
		});
	}

	std::string ItemManager::stringifyTextures() const {
		std::string result;

		for (int i = 0; i < std::ssize(potions); ++i) {
			result += std::format("{} {}\n", potions[i].id, assets->stringify(*potionTextures[i]));
		}

		boost::mp11::mp_for_each<boost::describe::describe_enumerators<EquipmentSlot>>([&](auto D) {
			int slotIndex = static_cast<int>(D.value);
			for (int i = 0; i < nEquipment(D.value); ++i) {
				result += std::format("{} {}\n", equipment[slotIndex][i].id, assets->stringify(*equipmentTextures[slotIndex][i]));
			}
		});

		return result;
	}

	void ItemManager::randomizeTextures() {
		std::ranges::shuffle(potionTextures, *randomEngine);
		boost::mp11::mp_for_each<boost::describe::describe_enumerators<EquipmentSlot>>([&](auto D) {
			std::ranges::shuffle(equipmentTextures[static_cast<int>(D.value)], *randomEngine);
		});
	}
}
