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

#include "render/Camera/Camera.hpp"
#include "render/draw/Hud.hpp"
#include "render/AssetManager.hpp"
#include "render/PlayerMap.hpp"

#include "util/Keyboard.hpp"
#include "util/Direction.hpp"

namespace core {
	PlayerController::PlayerController(std::shared_ptr<Actor> player_, 
		                               std::shared_ptr<util::Raycaster> raycaster_, 
		                               render::Context renderContext_) :
			player{player_}, raycaster{std::move(raycaster_)}, renderContext{renderContext_} {
		wantsSwap(false);
		isOnPlayerSide(true);
		shouldInterruptOnDelete(true);
		player_->world().player(player_);
	}

	void PlayerController::endTurn() noexcept {
		state = State::ENDED_TURN;
		renderContext.playerMap->clearSounds();
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
				if (event.key.shift) {
					if (tryDescentStairs())
						endTurn();
				} else {
					state = State::RESTING;
					renderContext.playerMap->clearSounds();
					player.lock()->endTurn();
				}
			}
			else if (util::isNumpad(event.key.code))
				for (ptrdiff_t i = 1; i <= 9; ++i)
					if (sf::Keyboard::isKeyPressed(util::numpad(i))) {
						if (player.lock()->tryMove(util::directions<int>[i - 1], true))
							endTurn();
						return;
					}
		} else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			auto player_ = player.lock();
			if (auto newCurrentSpell = render::clickedSpell(
				{event.mouseButton.x, event.mouseButton.y}, *renderContext.window, *player_
			)) {
				currentSpell_ = newCurrentSpell;
			} else if (currentSpell_) {
				core::Position<int> target{render::mouseTile({event.mouseButton.x, event.mouseButton.y}, 
					renderContext.camera->position(), *renderContext.window), player_->position().z};
				if (player_->spells()[*currentSpell_]->cast(*player_, target))
					endTurn();
			}
		} else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
			currentSpell_ = std::nullopt;
		}
	}

	bool PlayerController::act() {
		if (state == State::RESTING)
			if (player.lock()->hp() < player.lock()->maxHp() && !canSeeEnemy()) {
				player.lock()->endTurn();
				return true;
			} else {
				state = State::WAITING_INPUT;
				return false;
			}

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

	bool PlayerController::canSeeEnemy() const {
		auto player_ = player.lock();
		return std::ranges::any_of(player_->world().actors(), [this, &player_](const auto& actor) {
			return !actor->controller().isOnPlayerSide() 
				&& raycaster->canSee(player_->position(), actor->position());
		});
	}
}
