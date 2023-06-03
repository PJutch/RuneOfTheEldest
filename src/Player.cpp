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

#include "Player.hpp"

#include "World.hpp"

#include <iostream>

void Player::spawn() {
	nextTurn(0);
	hp(maxHp());

	world().addActor(shared_from_this());

	int level = 0;
	sf::Vector2i playerPos = world().dungeon()[level].randomPosition(*randomEngine, [this, level](sf::Vector2i pos, const Level&) {
		return world().isFree(make3D(pos, level));
	});
	position(make3D(playerPos, level));
}

void Player::handleEvent(sf::Event event) {
	if (state != State::WAITING_INPUT)
		return;

	if (event.type == sf::Event::KeyPressed)
		switch (event.key.code) {
		case sf::Keyboard::Numpad7: tryMove({ -1, -1 }); break;
		case sf::Keyboard::Numpad8: tryMove({ 0, -1 }); break;
		case sf::Keyboard::Numpad9: tryMove({ 1, -1 }); break;
		case sf::Keyboard::Numpad4: tryMove({ -1,  0 }); break;
		case sf::Keyboard::Numpad5: endTurn(); break;
		case sf::Keyboard::Numpad6: tryMove({ 1,  0 }); break;
		case sf::Keyboard::Numpad1: tryMove({ -1,  1 }); break;
		case sf::Keyboard::Numpad2: tryMove({ 0,  1 }); break;
		case sf::Keyboard::Numpad3: tryMove({ 1,  1 }); break;
		case sf::Keyboard::Comma:
			if (event.key.shift)
				tryAscentStairs();
			break;
		case sf::Keyboard::Period:
			if (event.key.shift)
				tryDescentStairs();
			break;
		}
}

bool Player::act() {
	if (state == State::ENDED_TURN) {
		state = State::WAITING_TURN;
		return true;
	}

	if (state == State::WAITING_TURN)
		state = State::WAITING_INPUT;
	return false;
}

void Player::tryAscentStairs() {
	if (std::optional<sf::Vector3i> newPos = world().dungeon().upStairs(position()))
		tryMoveTo(*newPos);
}

void Player::tryDescentStairs() {
	if (std::optional<sf::Vector3i> newPos = world().dungeon().downStairs(position()))
		tryMoveTo(*newPos);
}
