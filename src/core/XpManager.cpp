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
	XpManager::XpManager(std::shared_ptr<World> world_, 
						 std::shared_ptr<EffectManager> effects_, std::shared_ptr<SpellManager> spells_,
			             util::LoggerFactory& loggerFactory, util::RandomEngine& randomEngine_) :
		effects{std::move(effects_)}, spells{std::move(spells_)}, world{std::move(world_)},
		    randomEngine{&randomEngine_}, logger{loggerFactory.create("xp")} {
		for (const auto& effect : *effects)
			if (effect->isSkill())
				skills.push_back(effect.get());
	}

	void XpManager::generateLevelupChoices() const {
		logger->info("Generating levelup choices...");
		levelupChoices_.resize(3);
		for (auto& choice : levelupChoices_) {
			while (true) {
				if (std::bernoulli_distribution{0.5}(*randomEngine)) {
					auto iskill = std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(skills) - 1}(*randomEngine);
					const Effect* skill = skills[iskill];

					logger->info("Selected skill #{} \"{}\"", iskill, skill->name());
					choice.type = LevelUpChoice::Type::SKILL;
					choice.name = skill->name();
					choice.icon = &skill->icon();
					choice.result = [skill, world = world] {
						world->player().addEffect(skill->clone());
					};
					break;
				} else {
					auto ispell = std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(*spells) - 1}(*randomEngine);
					const Spell& spell = (*spells)[ispell];

					if (std::ranges::none_of(world->player().spells(), [&spell](const auto& other) {
						return other->name() == spell.name();
					})) {
						logger->info("Selected spell #{} \"{}\"", ispell, spell.name());
						choice.type = LevelUpChoice::Type::SPELL;
						choice.name = spell.name();
						choice.icon = &spell.icon();
						choice.result = [&spell, world = world] {
							world->player().addSpell(spell.clone());
						};
						break;
					}
				}
			}
		}
	}

	void XpManager::levelUp(const LevelUpChoice& choice) {
		logger->info("Finished level up");
		logger->info("Player selected {} \"{}\"", LevelUpChoice::typeName(choice.type), choice.name);

		choice.result();
		levelupChoices_.clear();

		xp -= xpUntilNextLvl;
		xpUntilNextLvl *= 2;
	}

	void XpManager::addXp(double dxp) {
		xp += dxp * world->player().xpMul();
	}
}
