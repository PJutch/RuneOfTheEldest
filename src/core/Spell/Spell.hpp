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

#include <string>
#include <memory>

namespace core {
	/// Spell that Actor can cast
	class Spell {
	public:
		Spell(const sf::Texture& newIcon, std::string_view newName) : icon_{&newIcon}, name_{newName} {}
		virtual ~Spell() = default;

		/// @brief Casts spell
		/// @param self Casting Actor
		/// @param target Tile selected by player
		/// @returns true if should end turn
		virtual bool cast(std::shared_ptr<Actor> self, core::Position<int> target) = 0;

		/// @brief tries to coninue casting spell
		/// @returns true if actually continued
		virtual bool continueCast() {
			return false;
		}

		/// interrupts continuous casting
		virtual void interrupt() {}

		/// Skill icon for level up menu
		[[nodiscard]] const sf::Texture& icon() const {
			return *icon_;
		}

		/// Skill name for level up menu
		[[nodiscard]] const std::string& name() const {
			return name_;
		}

		[[nodiscard]] virtual std::shared_ptr<Spell> clone() const = 0;
	private:
		const sf::Texture* icon_;
		std::string name_;
	};
}

#endif