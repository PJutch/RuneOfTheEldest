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

#ifndef SKILL_HPP_
#define SKILL_HPP_

#include <core/fwd.hpp>

namespace sf {
	class Texture;
}

#include <string>
#include <memory>

namespace core {
	/// Skill modifying Actor stats
	class Skill {
	public:
		Skill(const sf::Texture& newIcon, std::string_view newName) : icon_{&newIcon}, name_{newName} {}
		virtual ~Skill() = default;

		/// Actor regen multiplier
		virtual double regenMul() const {
			return 1;
		}

		/// Actor damage multiplier
		virtual double damageMul(const Actor& target) const {
			return 1;
		}

		/// @brief Bonus added to actor speed
		/// @details Actor turn delay is stats.turnDelay / sum(skill.speedBonus())
		virtual double speedBonus() const {
			return 0;
		}

		/// Multiplier for xp recieved by Actor
		virtual double xpMul() const {
			return 1;
		}

		/// @brief Actor max hp and hp multiplier
		/// @warning Actor hp are recomputed only on skill addition
		virtual double hpMul() const {
			return 1;
		}

		/// Added to actor accuracy
		virtual double accuracyBonus() const {
			return 0;
		}

		/// Added to actor avasion
		virtual double evasionBonus() const {
			return 0;
		}

		/// Copies this Skill
		virtual std::unique_ptr<Skill> clone() const = 0;

		/// Sets Actor owning this skill
		virtual void owner(std::weak_ptr<Actor> newOwner) {}

		/// Skill icon for level up menu
		[[nodiscard]] const sf::Texture& icon() const {
			return *icon_;
		}

		/// Skill name for level up menu
		[[nodiscard]] const std::string& name() const {
			return name_;
		}
	private:
		const sf::Texture* icon_;
		std::string name_;
	};
}

#endif