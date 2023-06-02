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

#include "AliveActor.hpp"

#include "World.hpp"

#include "Renderer.hpp"

#include "Event.hpp"
#include "geometry.hpp"

#include <SFML/System.hpp>

/// Player character
class Player : public AliveActor, public std::enable_shared_from_this<Player> {
public:
	Player(std::shared_ptr<World> world_, RandomEngine& randomEngine_) : 
		AliveActor{ 10, {0, 0, 0}, std::move(world_) }, randomEngine{ &randomEngine_ } {}

	void spawn();

	/// waits for player input
	bool act() final;

	/// notifies about events
	/// @details moves on WSAD
	///      \n try to ascent/descent stairs by <>
	void handleEvent(sf::Event event);

	void draw(Renderer& renderer) const final {
		renderer.draw(*this);
	}
private:
	enum class State {
		WAITING_TURN,
		WAITING_INPUT,
		ENDED_TURN
	};
	State state = State::WAITING_TURN;

	RandomEngine* randomEngine;

	void tryAscentStairs();
	void tryDescentStairs();

	void attack(Actor& actor) final {
		actor.beDamaged(1);
	}

	void endTurn() noexcept final {
		state = State::ENDED_TURN;
		delayNextTurn(1);
	}
};

#endif