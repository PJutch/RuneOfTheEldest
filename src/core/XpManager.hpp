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

#ifndef XP_MANAGER_HPP_
#define XP_MANAGER_HPP_

#include "Skill.hpp"
#include "RegenSkill.hpp"

#include "World.hpp"

#include "render/AssetManager.hpp"

#include "util/random.hpp"

namespace core {
	class XpManager {
	public:
		XpManager() = default;
		XpManager(std::shared_ptr<World> world, std::shared_ptr<render::AssetManager> assets, util::RandomEngine& randomEngine_);

		void addXp(double dxp) {
			xp += dxp;
		}

		bool canLevelUp() {
			return xp >= xpUntilNextLvl;
		}

		std::span<Skill const* const> availableSkills() {
			if (availableSkills_.empty())
				generateAvailableSkills();
			return availableSkills_;
		}

		void levelUp(const Skill* skill);

		double xpPercentUntilNextLvl() {
			return xp / xpUntilNextLvl;
		}

		void onGenerate() {
			xp = 0;
			xpUntilNextLvl = 1;
		}
	private:
		double xp = 0;
		double xpUntilNextLvl = 1;

		std::vector<std::unique_ptr<Skill>> skills;
		std::vector<const Skill*> availableSkills_;

		std::shared_ptr<World> world;
		util::RandomEngine* randomEngine = nullptr;

		void generateAvailableSkills();
	};
}

#endif