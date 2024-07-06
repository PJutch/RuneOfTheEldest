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

#include "core/fwd.hpp"
#include "core/DamageType.hpp"
#include "core/StatBooster.hpp"

#include "util/Exception.hpp"

namespace sf {
	class Texture;
}

#include <string>
#include <memory>
#include <optional>

namespace core {
	/// Effect modifying Actor stats
	class Effect : public StatBooster {
	public:
		Effect() {}

		virtual ~Effect() = default;

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

		/// Checks if can be removed by cancelation potion
		[[nodiscard]] virtual bool isCancelable() const {
			return false;
		}

		/// Copies this Skill
		virtual std::unique_ptr<Effect> clone() const = 0;

		/// Sets Actor owning this skill
		virtual void owner([[maybe_unused]] std::weak_ptr<Actor> newOwner) {}

		/// Skill icon for level up menu
		[[nodiscard]] virtual const sf::Texture& icon() const = 0;

		/// Skill name for dev usage
		[[nodiscard]] virtual const std::string& id() const = 0;

		/// Skill name for level up menu
		[[nodiscard]] virtual const std::string& name() const = 0;

		/// Checks if Effect is Skill
		[[nodiscard]] virtual bool isSkill() const {
			return false;
		}

		/// Checks if Effect should be drawn
		[[nodiscard]] virtual bool isVisible() const {
			return true;
		}

		class UnexpectedData : public util::RuntimeError {
		public:
			UnexpectedData(util::Stacktrace stacktrace = {}) : 
				util::RuntimeError{"Effect doesn't stores data but recieved some", std::move(stacktrace)} {}
		};

		/// Parses data from save file
		virtual void parseData(std::string_view data) {
			if (!data.empty()) {
				throw UnexpectedData{};
			}
		}

		/// Returns effect data to be saved
		[[nodiscard]] virtual std::optional<std::string> stringify() const {
			return id();
		}
	};
}

#endif