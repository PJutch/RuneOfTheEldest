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

namespace sf {
	class Texture;
}

#include <string>
#include <memory>

namespace core {
	class Skill {
	public:
		Skill(const sf::Texture& newIcon, std::string_view newName) : icon_{&newIcon}, name_{newName} {}
		virtual ~Skill() = default;

		virtual double regenMul() const = 0;
		virtual double damageMul() const = 0;
		virtual double turnDelayMul() const = 0;
		virtual double xpMul() const = 0;
		virtual double hpMul() const = 0;

		virtual std::unique_ptr<Skill> clone() const = 0;

		[[nodiscard]] const sf::Texture& icon() const {
			return *icon_;
		}

		[[nodiscard]] const std::string& name() const {
			return name_;
		}
	private:
		const sf::Texture* icon_;
		std::string name_;
	};
}

#endif