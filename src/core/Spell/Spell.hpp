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

#ifndef SPELL_HPP_
#define SPELL_HPP_

#include <core/fwd.hpp>
#include <core/DamageType.hpp>
#include <core/Position.hpp>

namespace sf {
	class Texture;
}

#include <SFML/Graphics/Color.hpp>

#include <string>
#include <memory>

namespace core {
	/// Spell that Actor can cast
	class Spell {
	public:
		Spell(const sf::Texture& newIcon, std::string_view newId, std::string_view newName) : 
			icon_{&newIcon}, id_{newId}, name_{ newName } {}
		virtual ~Spell() = default;

		/// Feedback for gui
		enum class CastResult {
			NOT_SUPPORTED, ///< Can't use on selected target
			FAILURE,       ///< Can't use in given conditions
			SUCCESS        ///< Used succesfully
		};

		/// @brief Casts spell
		/// @param self Casting Actor
		/// @param target Tile selected by player
		/// @returns Feedback for gui
		virtual CastResult cast([[maybe_unused]] std::shared_ptr<Actor> self, [[maybe_unused]] core::Position<int> target) {
			return CastResult::NOT_SUPPORTED;
		}

		/// @brief Casts spell
		/// @param self Casting Actor
		/// @returns Feedback for gui
		virtual CastResult cast([[maybe_unused]] std::shared_ptr<Actor> self) {
			return CastResult::NOT_SUPPORTED;
		}

		/// @brief tries to coninue casting spell
		/// @returns true if actually continued
		virtual bool continueCast() {
			return false;
		}

		/// interrupts continuous casting
		virtual void interrupt() {}

		/// Spell icon for level up menu
		[[nodiscard]] const sf::Texture& icon() const {
			return *icon_;
		}

		/// Spell name for dev use
		[[nodiscard]] const std::string& id() const {
			return id_;
		}

		/// Spell name for level up menu
		[[nodiscard]] const std::string& name() const {
			return name_;
		}

		[[nodiscard]] virtual std::shared_ptr<Spell> clone() const = 0;

		virtual void owner(std::weak_ptr<Actor>) {}

		virtual sf::Color frameColor() const {
			return sf::Color{128, 128, 128};
		}

		virtual void parseData(std::string_view data) {}

		virtual [[nodiscard]] std::string stringify() const {
			return id();
		}
	private:
		const sf::Texture* icon_;
		std::string id_;
		std::string name_;
	};
}

#endif