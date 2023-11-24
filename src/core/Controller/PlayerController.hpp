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

#ifndef PLAYER_CONTROLLER_HPP_
#define PLAYER_CONTROLLER_HPP_

#include "Controller.hpp"

#include "../fwd.hpp"
#include "../World.hpp"

#include "render/Context.hpp"
#include "render/PlayerMap.hpp"

#include "util/geometry.hpp"
#include "util/raycast.hpp"

#include <SFML/Window/Event.hpp>

#include <optional>

namespace core {
	/// Controlled by player Actor
	class PlayerController : public Controller {
	public:
		PlayerController(std::shared_ptr<Actor> player, 
						 std::shared_ptr<util::Raycaster> raycaster, 
						 std::shared_ptr<util::PathBuffer> pathBuffer,
						 render::Context renderContext);

		/// Waits for player input
		bool act() final;

		/// Moves on WSAD, tries to ascent/descent stairs by <>
		void handleEvent(sf::Event event) final;

		/// Adds sound to PlayerMap. Interrupts rest
		void handleSound(Sound sound) {
			renderContext.playerMap->handleSound(sound);
			if (state == State::RESTING)
				state = State::WAITING_TURN;
		}

		/// Interrupts rest
		void handleDamaged(Sound sound) {
			if (state == State::RESTING)
				state = State::WAITING_TURN;
		}

		std::optional<int> currentSpell() const final {
			return currentSpell_;
		}
	private:
		std::weak_ptr<Actor> player;
		std::shared_ptr<util::Raycaster> raycaster;
		std::shared_ptr<util::PathBuffer> pathBuffer;
		render::Context renderContext;

		enum class State {
			WAITING_TURN,
			WAITING_INPUT,
			ENDED_TURN,
			RESTING,
			TRAVELING
		};
		State state = State::WAITING_TURN;

		std::optional<int> currentSpell_;

		core::Position<int> travelTarget;

		bool tryAscentStairs();
		bool tryDescentStairs();

		bool canSeeEnemy() const;

		void endTurn() noexcept;

		bool moveToTarget();
	};
}

#endif