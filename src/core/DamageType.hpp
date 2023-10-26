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

#ifndef DAMAGE_TYPE_HPP_
#define DAMAGE_TYPE_HPP_

#include <array>
#include <string>

namespace core {
	/// Type of damage source
	enum class DamageType {
		PHYSICAL, ///< Normal attacks
		POISON,   ///< Poison effect
		TOTAL     ///< Technical enumerator. Should be last
	};

	static const int totalDamageTypes = static_cast<int>(DamageType::TOTAL);

	inline const std::array<const std::string, totalDamageTypes> damageTypeNames{
		"physical",
		"poison"
	};
}

#endif