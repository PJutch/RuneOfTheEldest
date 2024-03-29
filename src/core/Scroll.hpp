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

#ifndef SCROLL_HPP_
#define SCROLL_HPP_

#include "core/fwd.hpp"
#include "core/Item.hpp"
#include "core/Spell/Spell.hpp"
#include "core/Spell/Manager.hpp"
#include "core/ItemManager.hpp"

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
	/// Scroll that Actor can use to cast spell to cast spell without using mana
	class Scroll : public Item {
	public:
		Scroll(std::shared_ptr<Spell> spell_, std::shared_ptr<ItemManager> items_, 
			   std::shared_ptr<render::AssetManager> assets_, util::RandomEngine& randomEngine_) :
			Item{"scroll." + spell_->id()}, spell{std::move(spell_)}, 
			items{std::move(items_)}, assets{std::move(assets_)}, randomEngine{&randomEngine_} {}

		UsageResult use(core::Position<int> target) final {
			UsageResult result = spell->cast(target, false);
			if (result == UsageResult::SUCCESS) {
				identify();
				shouldDestroy_ = true;
			}
			return result;
		}

		UsageResult use(Item& target) final {
			UsageResult result = spell->cast(target, false);
			if (result == UsageResult::SUCCESS) {
				identify();
				shouldDestroy_ = true;
			}
			return result;
		}

		UsageResult use() final {
			UsageResult result = spell->cast(false);
			if (result == UsageResult::SUCCESS) {
				identify();
				shouldDestroy_ = true;
			}
			return result;
		}

		[[nodiscard]] std::shared_ptr<Spell> castedSpell() const final {
			return spell;
		}

		void identify() final {
			items->identify(id());
		}

		[[nodiscard]] const sf::Texture& icon() const final {
			return assets->scrollTexture(items->isIdentified(id()) ? spell->icon()
																   : assets->texture("resources/textures/Spell/unknown.png"));
		}

		[[nodiscard]] std::string name() const final {
			return items->isIdentified(id()) ? spell->name() + " scroll" : "Unknown scroll";
		}

		[[nodiscard]] std::unique_ptr<Item> clone() const final {
			auto result = std::make_unique<Scroll>(*this);
			result->spell = spell->clone();
			return result;
		}

		void owner(std::weak_ptr<Actor> actor) final {
			self = actor;
			spell->owner(std::move(actor));
		}

		[[nodiscard]] virtual bool shouldDestroy() const {
			return shouldDestroy_;
		}
	private:
		std::shared_ptr<Spell> spell;
		std::weak_ptr<Actor> self;
		bool shouldDestroy_ = false;

		std::shared_ptr<ItemManager> items;
		std::shared_ptr<render::AssetManager> assets;
		util::RandomEngine* randomEngine;
	};
}

#endif