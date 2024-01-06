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
		Potion(double hp_, std::string_view id, std::string_view name, std::shared_ptr<ItemManager> items_,
			   std::shared_ptr<render::AssetManager> assets_, util::RandomEngine& randomEngine_) :
			Item{id}, hp{hp_}, name_ {name}, icon_{&assets_->randomPotionTexture()},
			items{std::move(items_)}, assets{std::move(assets_)}, randomEngine{&randomEngine_} {}

		UsageResult use() final {
			self.lock()->heal(hp);
			shouldDestroy_ = true;
			return UsageResult::SUCCESS;
		}

		void identify() final {
			items->identify(id());
		}

		[[nodiscard]] const sf::Texture& icon() const final {
			return *icon_;
		}

		[[nodiscard]] std::string name() const final {
			return items->isIdentified(id()) ? name_ : "Unknown potion";
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
	private:
		double hp;

		std::weak_ptr<Actor> self;
		bool shouldDestroy_ = false;

		std::string name_;
		const sf::Texture* icon_;

		std::shared_ptr<ItemManager> items;
		std::shared_ptr<render::AssetManager> assets;
		util::RandomEngine* randomEngine;
	};
}

#endif