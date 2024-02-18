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

#ifndef EQUIPMENT_HPP_
#define EQUIPMENT_HPP_

#include "core/fwd.hpp"
#include "core/Item.hpp"
#include "core/StatBooster.hpp"
#include "core/StatBoosts.hpp"

#include "render/AssetManager.hpp"

#include "util/Exception.hpp"
#include "util/random.hpp"
#include "util/assert.hpp"
#include "util/enum.hpp"

namespace sf {
	class Texture;
}

#include <SFML/Graphics/Color.hpp>

#include <string>
#include <string_view>
#include <memory>

TROTE_DEFINE_ENUM(EquipmentSlot,
	RING
);

inline const auto equipmentSlotByName = util::makeNameMap<EquipmentSlot>();

namespace core {
	/// Potion that Actor can drink to get Effect or an instant bonus
	class Equipment : public Item, public StatBooster {
	public:
		struct Stats {
			StatBoosts boosts;
			EquipmentSlot slot;
			std::string id;
			std::string name; 
			const sf::Texture* icon = nullptr;
		};

		Equipment(Stats stats_,
				 std::shared_ptr<ItemManager> items_, std::shared_ptr<XpManager> xpManager_,
				 std::shared_ptr<render::AssetManager> assets_, util::RandomEngine& randomEngine_) :
			Item{stats_.id}, stats{stats_},
			items{std::move(items_)}, xpManager{std::move(xpManager_)}, assets{std::move(assets_)}, randomEngine{&randomEngine_} {}

		UsageResult use() final;

		void identify() final;

		[[nodiscard]] const sf::Texture& icon() const final;

		[[nodiscard]] std::string name() const final;

		[[nodiscard]] std::unique_ptr<Item> clone() const final {
			return std::make_unique<Equipment>(*this);
		}

		void owner(std::weak_ptr<Actor> actor) final {
			self = actor;
		}

		[[nodiscard]] virtual bool shouldDestroy() const {
			return false;
		}

		void onLoad() final;

		double regenBonus() const final {
			return stats.boosts.regenBonus;
		}

		double manaRegenBonus() const final {
			return stats.boosts.manaRegenBonus;
		}

		double damageBonus(const Actor&) const final {
			return stats.boosts.damageBonus;
		}

		double speedBonus() const final {
			return stats.boosts.speedBonus;
		}

		double accuracyBonus() const final {
			return stats.boosts.accuracyBonus;
		}

		double evasionBonus() const final {
			return stats.boosts.evasionBonus;
		}

		double xpMul() const final {
			return stats.boosts.xpMul;
		}

		double hpBonus() const final {
			return stats.boosts.hpBonus;
		}

		double manaBonus() const final {
			return stats.boosts.manaBonus;
		}

		double defenceBonus(DamageType damageType) const final {
			return stats.boosts.defenceBonuses[static_cast<size_t>(damageType)];
		}
	private:
		Stats stats;

		std::weak_ptr<Actor> self;

		std::shared_ptr<ItemManager> items;
		std::shared_ptr<XpManager> xpManager;
		std::shared_ptr<render::AssetManager> assets;
		util::RandomEngine* randomEngine;
	};
}

#endif