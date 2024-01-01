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
		Scroll(std::shared_ptr<Spell> spell_, const sf::Texture& icon_, 
			   std::shared_ptr<SpellManager> spells_, util::RandomEngine& randomEngine_) :
			Item{icon_, "scroll." + spell_->id(), spell_->name() + " scroll"}, 
			spell{std::move(spell_)}, spells{std::move(spells_)}, randomEngine{&randomEngine_} {}

		UsageResult use(std::shared_ptr<Actor> self, core::Position<int> target) final {
			UsageResult result = spell->cast(self, target, false);
			if (result == UsageResult::SUCCESS) {
				shouldDestroy_ = true;
			}
			return result;
		}

		UsageResult use([[maybe_unused]] std::shared_ptr<Actor> self) final {
			UsageResult result = spell->cast(self, false);
			if (result == UsageResult::SUCCESS) {
				shouldDestroy_ = true;
			}
			return result;
		}

		[[nodiscard]] std::shared_ptr<Spell> castedSpell() const final {
			return spell;
		}

		[[nodiscard]] std::unique_ptr<Item> clone() const final {
			auto result = std::make_unique<Scroll>(*this);
			result->spell = spell->clone();
			return result;
		}

		void owner(std::weak_ptr<Actor> actor) final {
			spell->owner(std::move(actor));
		}

		virtual [[nodiscard]] bool shouldDestroy() const {
			return shouldDestroy_;
		}
	private:
		std::shared_ptr<Spell> spell;
		bool shouldDestroy_ = false;

		std::shared_ptr<SpellManager> spells;
		util::RandomEngine* randomEngine;
	};
}

#endif