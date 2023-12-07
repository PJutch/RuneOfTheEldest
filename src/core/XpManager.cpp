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
	XpManager::XpManager(std::shared_ptr<World> world_, std::shared_ptr<EffectManager> effects_,
			             util::LoggerFactory& loggerFactory, util::RandomEngine& randomEngine_) :
			effects{std::move(effects_)}, world{std::move(world_)},
		    randomEngine{&randomEngine_}, logger{loggerFactory.create("xp")} {
		for (const auto& effect : *effects)
			if (effect->isSkill())
				skills.push_back(effect.get());
	}

	void XpManager::generateAvailableSkills() const {
		logger->info("Selecting available skills...");
		availableSkills_.resize(3);
		for (const Effect*& skill : availableSkills_) {
			auto iskill = std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(skills) - 1}(*randomEngine);
			skill = skills[iskill];
			
			logger->info("Selected skill #{} {}", iskill, skill->name());
		}
	}

	void XpManager::levelUp(const Effect* skill) {
		logger->info("Finished level up");
		logger->info("Player selected skill {}", skill->name());

		world->player().addEffect(skill->clone());
		availableSkills_.clear();

		xp -= xpUntilNextLvl;
		xpUntilNextLvl *= 2;
	}

	void XpManager::addXp(double dxp) {
		xp += dxp * world->player().xpMul();
	}
}
