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

class Renderer;

#include "geometry.hpp"

#include <SFML/Window.hpp>
#include <SFML/System.hpp>

/// Player character
class Player : public AliveActor, public std::enable_shared_from_this<Player> {
public:
	Player(std::shared_ptr<World> world_, RandomEngine& randomEngine_) : 
		AliveActor{ 10, 0.1, std::move(world_), &randomEngine_ } {}
	Player() : AliveActor{ 10, 0.1, nullptr, nullptr } {}

	void spawn();

	/// waits for player input
	bool act() final;

	/// notifies about events
	/// @details moves on WSAD
	///      \n try to ascent/descent stairs by <>
	void handleEvent(sf::Event event);

	[[nodiscard]] bool shouldInterruptOnDelete() const final {
		return true;
	}

	[[nodiscard]] bool isOnPlayerSide() const final {
		return true;
	}

	[[nodiscard]] bool wantsSwap() const noexcept final {
		return false;
	}

	void handleSwap() noexcept final {}

	void handleSound(Sound) noexcept final {}

	class DrawMemento : public AliveActor::DrawMemento {
	public:
		DrawMemento(const Player& player) : AliveActor::DrawMemento{ player } {}

		void draw(Renderer& renderer) const final;
	};

	[[nodiscard]] std::unique_ptr<Actor::DrawMemento> createDrawMemento() const final {
		return std::make_unique<DrawMemento>(*this);
	}
private:
	enum class State {
		WAITING_TURN,
		WAITING_INPUT,
		ENDED_TURN
	};
	State state = State::WAITING_TURN;

	void tryAscentStairs();
	void tryDescentStairs();

	void endTurn() noexcept {
		state = State::ENDED_TURN;
		wait(1);
	}

	void attack(Actor& actor) final {
		actor.beDamaged(1);
		world().makeSound({ Sound::Type::ATTACK, true, position() });
		endTurn();
	}

	void moveSucceed() final {
		world().makeSound({ Sound::Type::WALK, true, position() });
		endTurn();
	}
};

#endif