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

#include "PlayerController.hpp"

#include "../World.hpp"
#include "../Actor.hpp"

#include "util/Keyboard.hpp"

namespace core {
	PlayerController::PlayerController(std::shared_ptr<Actor> player_, std::shared_ptr<render::PlayerMap> map_) :
			player{ player_ }, map{ std::move(map_) } {
		wantsSwap(false);
		isOnPlayerSide(true);
		shouldInterruptOnDelete(true);
		player_->world().player(player_);
	}

	void PlayerController::endTurn() noexcept {
		state = State::ENDED_TURN;
		map->clearSounds();
		player.lock()->endTurn();
	}

	void PlayerController::handleEvent(sf::Event event) {
		if (state != State::WAITING_INPUT)
			return;

		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Numpad5)
				endTurn();
			else if (event.key.code == sf::Keyboard::Comma) {
				if (event.key.shift)
					if (tryAscentStairs())
						endTurn();
			}
			else if (event.key.code == sf::Keyboard::Period) {
				if (event.key.shift)
					if (tryDescentStairs())
						endTurn();
			}
			else if (util::isNumpad(event.key.code))
				for (ptrdiff_t i = 1; i <= 9; ++i)
					if (sf::Keyboard::isKeyPressed(util::numpad(i))) {
						if (player.lock()->tryMove(util::numpadDirections<int>[i - 1], true))
							endTurn();
						return;
					}
		}
	}

	bool PlayerController::act() {
		if (state == State::ENDED_TURN) {
			state = State::WAITING_TURN;
			return true;
		}

		if (state == State::WAITING_TURN)
			state = State::WAITING_INPUT;
		return false;
	}

	bool PlayerController::tryAscentStairs() {
		auto player_ = player.lock();
		if (std::optional<sf::Vector3i> newPos = player_->world().upStairs(player_->position()))
			return player_->tryMoveTo(*newPos, true);
		return false;
	}

	bool PlayerController::tryDescentStairs() {
		auto player_ = player.lock();
		if (std::optional<sf::Vector3i> newPos = player_->world().downStairs(player_->position()))
			return player_->tryMoveTo(*newPos, true);
		return false;
	}
}