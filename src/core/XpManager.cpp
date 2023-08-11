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

#include "XpManager.hpp"

#include "Actor.hpp"

namespace core {
	XpManager::XpManager(std::shared_ptr<World> world_, std::shared_ptr<render::AssetManager> assets, util::RandomEngine& randomEngine_) :
			world{std::move(world_)}, randomEngine{&randomEngine_} {
		skills.push_back(std::make_unique<RegenSkill>(2, assets->texture("resources/textures/RegenSkill.png")));
	}

	void XpManager::generateAvailableSkills() {
		availableSkills_.resize(3);
		for (const Skill*& skill : availableSkills_) {
			auto iskill = std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(skills) - 1}(*randomEngine);
			skill = skills[iskill].get();
		}
	}

	void XpManager::levelUp(const Skill* skill) {
		world->player().addSkill(skill->clone());
		xp -= xpUntilNextLvl;
		xpUntilNextLvl *= 2;
	}
}
