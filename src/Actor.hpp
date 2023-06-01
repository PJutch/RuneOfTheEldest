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

#include <SFML/System.hpp>

/// Abstract base for object that performs some actions in its turn
class Actor {
public:
	virtual ~Actor() = default;
	
	/// @brief Perform action in its turn
	/// @details Called in this actor turn.
	/// Can perform action and advance next turn or wait for user input
	/// @returns true if should pass turn to other actor, false if should wait
	virtual bool act() = 0;

	virtual sf::Vector3i position() const = 0;

	/// Gets time when actor's next turn begins
	int nextTurn() const noexcept {
		return nextTurn_;
	}
protected:
	Actor() = default;

	/// Add delay units to time before next turn
	void delayNextTurn(int delay) noexcept {
		nextTurn_ += delay;
	}
private:
	int nextTurn_ = 0;
};

#endif