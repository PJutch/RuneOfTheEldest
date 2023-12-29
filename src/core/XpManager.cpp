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

#include "util/parseKeyValue.hpp"

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
					int iskill = std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(skills) - 1}(*randomEngine);
					logger->info("Selected skill #{} \"{}\"", iskill, skills[iskill]->name());
					choice = skills[iskill];
					break;
				} else {
					auto ispell = std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(*spells) - 1}(*randomEngine);
					if (std::ranges::none_of(world->player().spells(), [&](const auto& other) {
						return other->name() == (*spells)[ispell].name();
					})) {
						logger->info("Selected spell #{} \"{}\"", ispell, (*spells)[ispell].name());
						choice = &(*spells)[ispell];
						break;
					}
				}
			}
		}
	}

	void XpManager::levelUp(const LevelUpChoice& choice) {
		logger->info("Finished level up");
		std::visit([&]<typename Choice>(Choice choice) {
			if constexpr (std::same_as<Choice, const Effect*>) {
				logger->info("Player selected skill \"{}\"", choice->name());
				world->player().addEffect(choice->clone());
			} else if constexpr (std::same_as<Choice, const Spell*>) {
				logger->info("Player selected spell \"{}\"", choice->name());
				world->player().addSpell(choice->clone());
			} else {
				TROTE_ASSERT(false, "unreachable");
			}
		}, choice);

		levelupChoices_.clear();

		xp -= xpUntilNextLvl;
		xpUntilNextLvl *= 2;
	}

	void XpManager::addXp(double dxp) {
		xp += dxp * world->player().xpMul();
	}

	void XpManager::parse(std::string_view data) {
		util::KeyValueVisitor visitor;
		visitor.key("xp").unique().required().callback([&](std::string_view data) {
			xp = util::parseReal(data);
		});
		visitor.key("levelCost").unique().required().callback([&](std::string_view data) {
			xpUntilNextLvl = util::parseReal(data);
		});
		visitor.key("choice.skill").callback([&](std::string_view data) {
			levelupChoices_.push_back(effects->findEffect(data));
		});
		visitor.key("choice.spell").callback([&](std::string_view data) {
			levelupChoices_.push_back(spells->findSpell(data).get());
		});

		util::forEackKeyValuePair(data, visitor);
		visitor.validate();
	}

	[[nodiscard]] std::string XpManager::stringify() const {
		std::string result = std::format("xp {}\nlevelCost {}\n", xp, xpUntilNextLvl);
		for (const LevelUpChoice& choice : levelupChoices_) 
			std::visit([&]<typename Choice>(Choice choice) {
				if constexpr (std::same_as<Choice, const Effect*>) {
					result += std::format("choice.skill {}\n", choice->id());
				} else if constexpr (std::same_as<Choice, const Spell*>) {
					result += std::format("choice.spell {}\n", choice->id());
				} else {
					TROTE_ASSERT(false, "unreachable");
				}
			}, choice);
		return result;
	}
}
