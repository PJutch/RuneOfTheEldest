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

#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "Actor.hpp"

#include "World.hpp"

#include "Event.hpp"
#include "geometry.hpp"

#include <SFML/System.hpp>

/// Player character
class Player : public Actor, public std::enable_shared_from_this<Player> {
public:
	Player(std::shared_ptr<World> world_, RandomEngine& randomEngine_) : 
		world(std::move(world_)), randomEngine{&randomEngine_ } {}

	void spawn();

	/// Returns position as (x, y, level)
	sf::Vector3i position() const noexcept final {
		return position_;
	}

	/// Sets position to (x, y) and level to z
	void position(sf::Vector3i newPosition) noexcept {
		position_ = newPosition;
	}

	/// waits for player input
	bool act() final;

	/// notifies about events
	/// @details moves on WSAD
	///      \n try to ascent/descent stairs by <>
	void handleEvent(sf::Event event);
private:
	enum class State {
		WAITING_TURN,
		WAITING_INPUT,
		ENDED_TURN
	};
	State state = State::WAITING_TURN;
	
	sf::Vector3i position_;

	std::shared_ptr<World> world;
	RandomEngine* randomEngine;

	void tryMoveTo(sf::Vector3i newPosition);

	void tryMoveTo(sf::Vector2i newPosition) {
		tryMoveTo(make3D(newPosition, position().z));
	}

	void tryMove(sf::Vector2i offset) {
		tryMoveTo(getXY(position()) + offset);
	}

	void tryAscentStairs();
	void tryDescentStairs();
};

#endif