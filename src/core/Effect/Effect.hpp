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

#ifndef EFFECT_HPP_
#define EFFECT_HPP_

#include <core/fwd.hpp>
#include <core/DamageType.hpp>

namespace sf {
	class Texture;
}

#include <string>
#include <memory>

namespace core {
	/// Effect modifying Actor stats
	class Effect {
	public:
		Effect(const sf::Texture& newIcon, std::string_view newName, bool isSkill) : 
			icon_{&newIcon}, name_{newName}, isSkill_{isSkill} {}

		virtual ~Effect() = default;

		/// Adds regenBonus() * stats.regen to actor regen
		virtual double regenBonus() const {
			return 0;
		}

		/// Adds manaRegenBonus() * stats.manaRegen to actor regen
		virtual double manaRegenBonus() const {
			return 0;
		}

		/// Adds damageBonus(target) * stats.damage to dealed damage
		virtual double damageBonus([[maybe_unused]] const Actor& target) const {
			return 0;
		}

		/// @brief Bonus added to actor speed
		/// @details Actor turn delay is stats.turnDelay / sum(skill.speedBonus())
		virtual double speedBonus() const {
			return 0;
		}

		/// Multiplier for xp recieved by Actor
		virtual double xpMul() const {
			return 1;
		}

		/// @brief Adds hpBonus() * stats.maxHp to actor maxHp
		/// @warning Actor hp are recomputed only on skill addition
		virtual double hpBonus() const {
			return 0;
		}

		/// @brief Adds manaBonus() * stats.maxMana to actor maxMana
		/// @warning Actor mana is recomputed only on skill addition
		virtual double manaBonus() const {
			return 0;
		}

		/// Added to actor accuracy
		virtual double accuracyBonus() const {
			return 0;
		}

		/// Added to actor avasion
		virtual double evasionBonus() const {
			return 0;
		}

		/// Added to actor defence. Gets damage type
		virtual double defenceBonus(DamageType) const {
			return 0;
		}

		/// Applies effects over time. Called in the end of the turn
		virtual void update([[maybe_unused]] double time) {}

		/// Called after loading all the effects
		virtual void init(const EffectManager&) {}

		/// Called when owner attacks another Actor
		virtual void onAttack(Actor&) const {}

		/// Checks if effect should end
		virtual bool shouldBeRemoved() const {
			return false;
		}

		/// Copies this Skill
		virtual std::unique_ptr<Effect> clone() const = 0;

		/// Sets Actor owning this skill
		virtual void owner([[maybe_unused]] std::weak_ptr<Actor> newOwner) {}

		/// Skill icon for level up menu
		[[nodiscard]] const sf::Texture& icon() const {
			return *icon_;
		}

		/// Skill name for level up menu
		[[nodiscard]] const std::string& name() const {
			return name_;
		}

		/// Checks if Effect is Skill
		[[nodiscard]] bool isSkill() {
			return isSkill_;
		}
	private:
		const sf::Texture* icon_;
		std::string name_;
		bool isSkill_;
	};
}

#endif