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

#include "util/enum.hpp"

#include <boost/describe.hpp>

#include <array>
#include <string>

namespace core {
	/// Type of damage source
	TROTE_DEFINE_ENUM(DamageType,
		PHYSICAL,    ///< Normal attacks
		POISON,      ///< Poison effect
		MAGIC,       ///< Spell
		ELECTRICITY, ///< Lightning spells
		LIGHT,       ///< Holy spells
		DEATH,       ///< Dark spells
		FIRE,        ///< Fire spells
		COLD         ///< Freezing spells
	)

	inline const auto damageTypeByName = util::makeNameMap<DamageType>();
}

#endif