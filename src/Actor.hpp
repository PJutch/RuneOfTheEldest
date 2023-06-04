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

class Renderer;

#include <SFML/System.hpp>

/// Abstract base for object that performs some actions in its turn
class Actor {
public:
	virtual ~Actor() = default;

	/// Actor position in world
	virtual [[nodiscard]] sf::Vector3i position() const = 0;
	
	/// @brief Perform action in its turn
	/// @details Called in this actor turn.
	/// Can perform action and advance next turn or wait for user input
	/// @returns true if should pass turn to other actor, false if should wait
	virtual bool act() = 0;

	/// @brief Gets time when actor's next turn begins
	/// @warning Returned time should change only in Actor's turn
	virtual [[nodiscard]] int nextTurn() const = 0;
	
	/// Checks if Actor should be removed
	virtual [[nodiscard]] bool isAlive() const = 0;

	/// If true interrupts Actor processing in queue and continue Game loop when this Actor deleted
	virtual [[nodiscard]] bool shouldInterruptOnDelete() const = 0;

	/// Damages Actor by given HPs
	virtual void beDamaged(double damage) = 0;

	/// Draw an actor using a renderer. Basically a visitor pattern
	virtual void draw(Renderer& renderer) const = 0;
};

#endif