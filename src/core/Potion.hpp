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

#ifndef POTION_HPP_
#define POTION_HPP_

#include "core/fwd.hpp"
#include "core/Item.hpp"
#include "core/Spell/Spell.hpp"
#include "core/Spell/Manager.hpp"
#include "core/ItemManager.hpp"
#include "core/Actor.hpp"

#include "render/AssetManager.hpp"

#include "util/Exception.hpp"
#include "util/random.hpp"
#include "util/assert.hpp"

namespace sf {
	class Texture;
}

#include <SFML/Graphics/Color.hpp>

#include <string>
#include <string_view>
#include <memory>

namespace core {
	/// Potion that Actor can drink to get Effect or an instant bonus
	class Potion : public Item {
	public:
		struct Stats {
			double hp = 0;
			double mana = 0;
			double xp = 0;
			const core::Effect* effect = nullptr;
			bool cancelEffects = false;

			std::string_view id; 
			std::string_view name; 
			const sf::Texture* label = nullptr;
		};

		Potion(Stats stats_, 
			   std::shared_ptr<ItemManager> items_, std::shared_ptr<XpManager> xpManager_,
			   std::shared_ptr<render::AssetManager> assets_, util::RandomEngine& randomEngine_) :
			Item{stats_.id}, stats{stats_}, icon_{&assets_->randomPotionBaseTexture()},
			items{std::move(items_)}, xpManager{std::move(xpManager_)}, assets{std::move(assets_)}, randomEngine{&randomEngine_} {}

		UsageResult use() final {
			self.lock()->heal(stats.hp);
			self.lock()->restoreMana(stats.mana);
			xpManager->addXp(stats.xp);
			if (stats.cancelEffects) {
				self.lock()->cancelEffects();
			}
			if (stats.effect) {
				self.lock()->addEffect(stats.effect->clone());
			}
			
			identify();
			shouldDestroy_ = true;
			return UsageResult::SUCCESS;
		}

		void identify() final {
			items->identify(id());
		}

		[[nodiscard]] const sf::Texture& icon() const final {
			return items->isIdentified(id()) ? assets->potionTexture(*icon_, *stats.label) : *icon_;
		}

		[[nodiscard]] std::string name() const final {
			return items->isIdentified(id()) ? std::string{stats.name} : "Unknown potion";
		}

		[[nodiscard]] std::unique_ptr<Item> clone() const final {
			return std::make_unique<Potion>(*this);
		}

		void owner(std::weak_ptr<Actor> actor) final {
			self = actor;
		}

		[[nodiscard]] virtual bool shouldDestroy() const {
			return shouldDestroy_;
		}

		void parseTextureData(std::string_view data) final {
			icon_ = &assets->parse(data);
		}

		[[nodiscard]] std::optional<std::string> stringifyTextureData() const final {
			return assets->stringify(*icon_);
		}

		void onLoad() final {
			icon_ = &dynamic_cast<Potion&>(*items->findItem(id())).baseIcon();
		}

		void onTextureRandomize() final {
			icon_ = &assets->randomPotionBaseTexture();
		}

		const sf::Texture& baseIcon() const noexcept {
			return *icon_;
		}
	private:
		Stats stats;

		std::weak_ptr<Actor> self;
		bool shouldDestroy_ = false;

		const sf::Texture* icon_;

		std::shared_ptr<ItemManager> items;
		std::shared_ptr<XpManager> xpManager;
		std::shared_ptr<render::AssetManager> assets;
		util::RandomEngine* randomEngine;
	};
}

#endif