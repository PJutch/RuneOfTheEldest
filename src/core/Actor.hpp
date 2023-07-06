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

#ifndef ACTOR_HPP_
#define ACTOR_HPP_

#include "Sound.hpp"
#include "AiState.hpp"

namespace render {
	class Renderer;
}

#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector3.hpp>

#include <memory>

namespace core {
	/// Abstract base for object that performs some actions in its turn
	class Actor {
	public:
		virtual ~Actor() = default;

		/// Actor position in world
		[[nodiscard]] virtual sf::Vector3i position() const = 0;

		/// Sets position in world
		virtual void position(sf::Vector3i newPosition) = 0;

		/// @brief Perform action in its turn
		/// @details Called in this actor turn.
		/// Can perform action and advance next turn or wait for user input
		/// @returns true if should pass turn to other actor, false if should wait
		virtual bool act() = 0;

		/// @brief Gets time when actor's next turn begins
		/// @warning Returned time should change only in Actor's turn
		[[nodiscard]] virtual int nextTurn() const = 0;

		/// Checks if Actor should be removed
		[[nodiscard]] virtual bool isAlive() const = 0;

		/// If true interrupts Actor processing in queue and continue Game loop when this Actor deleted
		[[nodiscard]] virtual bool shouldInterruptOnDelete() const = 0;

		/// Checks if Actor is player ally
		[[nodiscard]] virtual bool isOnPlayerSide() const = 0;

		/// Checks if Actor can swap with AI controlled Actor
		[[nodiscard]] virtual bool wantsSwap() const = 0;

		/// Called after swapping with another Actor
		virtual void handleSwap() = 0;

		/// Damages Actor by given HPs
		virtual void beDamaged(double damage) = 0;

		/// called when Actor hears a sound
		virtual void handleSound(Sound sound) = 0;

		/// Gets Actor HP
		[[nodiscard]] virtual double hp() const = 0;

		/// Gets max possible HP
		[[nodiscard]] virtual double maxHp() const = 0;

		virtual AiState aiState() const = 0;

		[[nodiscard]] virtual const sf::Texture* texture() const = 0;
	};
}

#endif