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

#ifndef ITEM_HPP_
#define ITEM_HPP_

#include "fwd.hpp"
#include "Position.hpp"
#include "Usable.hpp"

#include "Spell/Spell.hpp"

#include "util/Exception.hpp"

namespace sf {
	class Texture;
}

#include <SFML/Graphics/Color.hpp>

#include <string>
#include <string_view>
#include <memory>

namespace core {
	/// Item that Actor can carry around and use
	class Item {
	public:
		Item(std::string_view newId) : id_{newId} {}
		virtual ~Item() = default;

		/// @brief Uses item
		/// @param target Tile selected by player
		/// @returns Feedback for gui
		virtual UsageResult use([[maybe_unused]] core::Position<int> target) {
			return UsageResult::NOT_SUPPORTED;
		}

		/// @brief Uses item
		/// @param target Item selected by player
		/// @returns Feedback for gui
		virtual UsageResult use([[maybe_unused]] Item& target) {
			return UsageResult::NOT_SUPPORTED;
		}

		/// @brief Uses item
		/// @returns Feedback for gui
		virtual UsageResult use() {
			return UsageResult::NOT_SUPPORTED;
		}

		/// Spell for Actor to start casting
		[[nodiscard]] virtual std::shared_ptr<Spell> castedSpell() const {
			return nullptr;
		}

		/// Reveals this item type to player
		virtual void identify() = 0;

		/// Item name for dev use
		[[nodiscard]] const std::string& id() const {
			return id_;
		}

		/// Item icon for ingame display
		[[nodiscard]] virtual const sf::Texture& icon() const = 0;

		/// Item name for ingame display
		[[nodiscard]] virtual std::string name() const = 0;

		[[nodiscard]] virtual bool shouldDestroy() const = 0;

		[[nodiscard]] virtual std::unique_ptr<Item> clone() const = 0;

		virtual void owner(std::weak_ptr<Actor>) {}

		virtual sf::Color frameColor() const {
			return sf::Color{128, 128, 128};
		}

		class UnexpectedData : public util::RuntimeError {
		public:
			UnexpectedData(util::Stacktrace stacktrace = {}) :
				util::RuntimeError{"Item doesn't stores data but recieved some", std::move(stacktrace)} {}
		};

		/// Parses data from save file
		virtual void parseData(std::string_view data) {
			if (!data.empty()) {
				throw UnexpectedData{};
			}
		}

		[[nodiscard]] virtual std::string stringifyData() const {
			return "";
		}
	private:
		std::string id_;
	};
}

#endif