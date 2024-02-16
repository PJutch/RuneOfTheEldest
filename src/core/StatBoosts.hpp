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

#ifndef STAT_BOOSTS_HPP_
#define STAT_BOOSTS_HPP_

#include "core/DamageType.hpp"

namespace core {
	struct StatBoosts {
		double regenBonus = 0;
		double manaRegenBonus = 0;

		double speedBonus = 0;
		double accuracyBonus = 0;
		double evasionBonus = 0;

		double xpMul = 1;

		double hpBonus = 0;
		double manaBonus = 0;

		double damageBonus = 0;

		std::array<double, util::nEnumerators<DamageType>> defenceBonuses{};
	};

	/// @brief Loads bonuses
	/// @param params Map with params. Consumed
	/// @throws anonymous::UnknowParams if some of params are unknown
	/// @todo Choose a better place
	StatBoosts loadBonuses(std::unordered_map<std::string, std::string>& params);
}

#endif