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
#include "util/pathfinding.hpp"

namespace core {
	PlayerController::PlayerController(std::shared_ptr<Actor> player_, 
		                               std::shared_ptr<util::Raycaster> raycaster_,
		                               render::Context renderContext_) :
			player{player_}, raycaster{std::move(raycaster_)}, pathBuffer{std::make_unique<util::PathBuffer>()},
			renderContext{renderContext_}, travelTarget{player_->position()} {
		wantsSwap(false);
		isOnPlayerSide(true);
		shouldInterruptOnDelete(true);
		player_->world().player(player_);
	}

	void PlayerController::endTurn(std::shared_ptr<Spell> newCastSpell) noexcept {
		if (castSpell) {
			castSpell->interrupt();
		}
		castSpell = std::move(newCastSpell);

		state = State::ENDED_TURN;
		renderContext.playerMap->clearSounds();
		player.lock()->endTurn();
	}

	void PlayerController::handleEvent(sf::Event event) {
		if (state != State::WAITING_INPUT)
			return;

		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Numpad5) {
				if (castSpell) {
					castSpell->continueCast();
					state = State::ENDED_TURN;
					renderContext.playerMap->clearSounds();
					player.lock()->endTurn();
					return;
				}
				endTurn();
			} else if (event.key.code == sf::Keyboard::Comma) {
				if (event.key.shift)
					if (tryAscentStairs())
						endTurn();
			} else if (event.key.code == sf::Keyboard::Period) {
				if (event.key.shift) {
					if (tryDescentStairs())
						endTurn();
				} else {
					state = State::RESTING;
					renderContext.playerMap->clearSounds();
					player.lock()->endTurn();
				}
			} else if (event.key.code == sf::Keyboard::O) {
				state = State::EXPLORING;
				explore();
			} else if (util::isNumpad(event.key.code)) {
				for (ptrdiff_t i = 1; i <= 9; ++i)
					if (sf::Keyboard::isKeyPressed(util::numpad(i))) {
						if (player.lock()->tryMove(util::directions<int>[i - 1], true))
							endTurn();
						return;
					}
			}
		} else if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				handleClick({event.mouseButton.x, event.mouseButton.y});
			} else if (event.mouseButton.button == sf::Mouse::Right) {
				currentSpell_ = std::nullopt;
			}
		}
	}

	void PlayerController::handleClick(sf::Vector2i clickPos) {
		auto player_ = player.lock();
		if (auto newCurrentSpell = render::clickedSpell(clickPos, *renderContext.window, *player_)) {
			auto spell = player_->spells()[*newCurrentSpell];
			switch (spell->cast(player_)) {
			case Spell::CastResult::SUCCESS:
				endTurn(spell);
				break;
			case Spell::CastResult::FAILURE:
				break;
			case Spell::CastResult::NOT_SUPPORTED:
				currentSpell_ = newCurrentSpell;
				break;
			default:
				TROTE_ASSERT(false, "unreachable");
			}
		} else if (currentSpell_) {
			auto target = render::mouseTile(clickPos, renderContext.camera->position(), *renderContext.window);
			auto spell = player_->spells()[*currentSpell_];
			if (spell->cast(player_, target) == Spell::CastResult::SUCCESS) {
				endTurn(spell);
			}
		} else {
			if (!canSeeEnemy()) {
				auto newTarget = render::mouseTile(clickPos, renderContext.camera->position(), *renderContext.window);
				if (player_->world().tiles().isValidPosition(static_cast<sf::Vector3i>(newTarget))) {
					state = State::TRAVELING;
					travelTarget = newTarget;
					moveToTarget();
				}
			}
		}
	}

	bool PlayerController::moveToTarget() {
		auto player_ = player.lock();
		sf::Vector3i nextStep_ = util::nextStep(player_->world(), player_->position(),
				static_cast<sf::Vector3i>(travelTarget), *pathBuffer, 
				[&playerMap = *renderContext.playerMap](const core::World& world, sf::Vector3i pos) {
			return isPassable(world.tiles()[pos]) && playerMap.tileState(pos) != render::PlayerMap::TileState::UNSEEN;
		});

		if (player_->tryMove(nextStep_, false)) {
			renderContext.playerMap->update();
			player_->endTurn();
			return true;
		} else {
			state = State::WAITING_INPUT;
			return false;
		}
	}

	bool PlayerController::explore() {
		auto player_ = player.lock();
		if (auto nextStep = util::nextExploreStep(player_->world(), player_->position(), *pathBuffer,
			[&playerMap = *renderContext.playerMap](const core::World&, sf::Vector3i pos) {
			return playerMap.tileState(pos) == render::PlayerMap::TileState::UNSEEN;
		})) {
			if (player_->tryMove(*nextStep, false)) {
				renderContext.playerMap->update();
				player_->endTurn();
				return true;
			} else {
				state = State::WAITING_INPUT;
				return false;
			}
		} else {
			return false;
		}
	}

	bool PlayerController::act() {
		switch (state) {
		case State::RESTING: 
			if (player.lock()->hp() < player.lock()->maxHp() && !canSeeEnemy()) {
				player.lock()->endTurn();
				return true;
			} else {
				state = State::WAITING_INPUT;
				return false;
			}
		case State::TRAVELING:
			if (!canSeeEnemy()) {
				return moveToTarget();
			} else {
				state = State::WAITING_INPUT;
				return false;
			}
		case State::EXPLORING:
			if (!canSeeEnemy()) {
				return explore();
			} else {
				state = State::WAITING_INPUT;
				return false;
			}
		case State::ENDED_TURN:
			state = State::WAITING_TURN;
			return true;
		case State::WAITING_TURN:
			state = State::WAITING_INPUT;
			return false;
		default:
			return false;
		}
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
