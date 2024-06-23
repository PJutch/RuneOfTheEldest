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

#include "StatBoosts.hpp"

#include "util/parse.hpp"
#include "util/Map.hpp"

namespace core {
	namespace {
		std::string unknownParamsMessage(std::unordered_map<std::string, std::string> params) {
			std::string message = "Unknown params: ";
			for (auto [name, value] : params)
				message += std::format("\"{}\" ", name);
			return message;
		}

		class UnknownParamsError : public util::RuntimeError {
		public:
			UnknownParamsError(std::unordered_map<std::string, std::string> params,
				util::Stacktrace currentStacktrace = {}) noexcept :
				RuntimeError{unknownParamsMessage(params), std::move(currentStacktrace)} {}
		};
	}

	StatBoosts loadBonuses(std::unordered_map<std::string, std::string>& params) {
		double regenBonus = 0;
		double manaRegenBonus = 0;
		double speedBonus = 0;
		double accuracyBonus = 0;
		double evasionBonus = 0;
		double xpMul = 1;
		double hpBonus = 0;
		double manaBonus = 0;

		if (auto v = util::getAndErase(params, "hpBonus"))
			hpBonus = util::parseReal(*v);

		if (auto v = util::getAndErase(params, "manaBonus"))
			manaBonus = util::parseReal(*v);

		if (auto v = util::getAndErase(params, "regenBonus"))
			regenBonus = util::parseReal(*v);

		if (auto v = util::getAndErase(params, "manaRegenBonus"))
			manaRegenBonus = util::parseReal(*v);

		if (auto v = util::getAndErase(params, "speed"))
			speedBonus = util::parseReal(*v);

		if (auto v = util::getAndErase(params, "accuracy"))
			accuracyBonus = util::parseReal(*v);

		if (auto v = util::getAndErase(params, "evasion"))
			evasionBonus = util::parseReal(*v);

		if (auto v = util::getAndErase(params, "xpMul"))
			xpMul = util::parseReal(*v);

		util::EnumMap<DamageType, double> defenceBonuses;
		defenceBonuses.fill(0);

		boost::mp11::mp_for_each<boost::describe::describe_enumerators<DamageType>>([&](auto D) {
			using namespace std::literals;
			if (auto v = util::getAndErase(params, util::toLower(D.name) + "DefenceBonus"s))
				defenceBonuses[D.value] = util::parseReal(*v);
		});

		double damageBonus = 0;
		if (auto v = util::getAndErase(params, "damageBonus"))
			damageBonus = util::parseReal(*v);

		if (!params.empty())
			throw UnknownParamsError{params};

		return {
			regenBonus, manaRegenBonus, speedBonus, accuracyBonus, evasionBonus,
			xpMul, hpBonus, manaBonus, damageBonus, defenceBonuses
		};
	}
}