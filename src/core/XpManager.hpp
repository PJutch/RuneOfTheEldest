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

#include "EffectManager.hpp"
#include "Spell/Manager.hpp"

#include "World.hpp"

#include "util/random.hpp"
#include "util/log.hpp"

namespace core {
	/// Manages player xp and leveling up
	class XpManager {
	public:
		struct LevelUpChoice {
			enum class Type {
				SKILL,
				SPELL
			};

			static std::string_view typeName(Type type) {
				switch (type) {
				case Type::SKILL: return "skill";
				case Type::SPELL: return "spell";
				default: TROTE_ASSERT(false, "Unreachable");
				}
			}

			Type type;
			std::string_view name;
			const sf::Texture* icon;
			std::function<void()> result;
		};

		XpManager() = default;
		XpManager(std::shared_ptr<World> world_) : world{std::move(world_)} {}
		XpManager(std::shared_ptr<World> world, 
			      std::shared_ptr<EffectManager> effects, std::shared_ptr<SpellManager> spells,
			      util::LoggerFactory& loggerFactory, util::RandomEngine& randomEngine_);

		/// Add given amount of xp to player
		void addXp(double dxp);

		/// Checks if player has enough xp to level up
		bool canLevelUp() const noexcept {
			return xp >= xpUntilNextLvl;
		}

		/// @brief Skills and spells for player to choose from
		/// @details Randomly selects 3 skills or spells for player.
		/// Return same skills and spells on each call.
		/// Resets on level up.
		std::span<LevelUpChoice const> levelupChoices() const {
			if (levelupChoices_.empty())
				generateLevelupChoices();
			return levelupChoices_;
		}

		/// @brief Levels up player
		/// @details Add clone of selected skill or spell to player.
		/// Resets available choices.
		/// Increases xp requirements for next level.
		void levelUp(const LevelUpChoice& choice);

		/// Returns colected percent of xp for next level
		double xpPercentUntilNextLvl() const noexcept {
			return xp / xpUntilNextLvl;
		}

		/// Resets xp and xp requirements
		void onGenerate() {
			xp = 0;
			xpUntilNextLvl = 1;
		}
	private:
		double xp = 0;
		double xpUntilNextLvl = 1;

		std::shared_ptr<EffectManager> effects;
		std::shared_ptr<SpellManager> spells;
		std::vector<const Effect*> skills;
		mutable std::vector<LevelUpChoice> levelupChoices_;

		std::shared_ptr<World> world;
		util::RandomEngine* randomEngine = nullptr;

		std::shared_ptr<spdlog::logger> logger;

		void generateLevelupChoices() const;
	};
}

#endif