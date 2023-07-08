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

#ifndef CONTROLLER_HPP_
#define CONTROLLER_HPP_

#include "AiState.hpp"
#include "Sound.hpp"

#include <SFML/Window/Event.hpp>

namespace core {
	/// Controlls Actor actions
	class Controller {
	public:
		/// @brief Perform action in its turn
		/// @details Called in this actor turn.
		/// Can perform action and advance next turn or wait for user input
		/// @returns true if should pass turn to other actor, false if should waits
		virtual bool act() = 0;

		/// Notifies about events
		virtual void handleEvent(sf::Event event) {}

		/// If true interrupts Actor processing in queue and continue Game loop when this Actor deleted
		[[nodiscard]] bool shouldInterruptOnDelete() const noexcept {
			return shouldInterruptOnDelete_;
		}

		/// Checks if Actor is player ally
		[[nodiscard]] bool isOnPlayerSide() const noexcept {
			return isOnPlayerSide_;
		}

		/// Checks if allies can swap with this Actor
		[[nodiscard]] bool wantsSwap() const noexcept {
			return wantsSwap_;
		}

		/// Called after swapping with another Actor
		virtual void handleSwap() noexcept {}

		/// Called when Actor hears a sound
		virtual void handleSound(Sound sound) noexcept {}

		[[nodiscard]] AiState aiState() const noexcept {
			return aiState_;
		}
	protected:
		void aiState(AiState newAiState) noexcept {
			aiState_ = newAiState;
		}

		void wantsSwap(bool newWantsSwap) noexcept {
			wantsSwap_ = newWantsSwap;
		}

		void isOnPlayerSide(bool newIsOnPlayerSide) noexcept {
			isOnPlayerSide_ = newIsOnPlayerSide;
		}

		void shouldInterruptOnDelete(bool newShouldInterruptOnDelete) noexcept {
			shouldInterruptOnDelete_ = newShouldInterruptOnDelete;
		}
	private:
		AiState aiState_ = AiState::NONE;
		bool wantsSwap_ = true;
		bool isOnPlayerSide_ = false;
		bool shouldInterruptOnDelete_ = false;
	};
}

#endif