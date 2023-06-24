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

#include "Renderer.hpp"

#include "Keyboard.hpp"

#include <iostream>

void Player::spawn() {
	nextTurn(0);
	hp(maxHp());

	world().addActor(shared_from_this());

	int level = 0;
	sf::Vector2i playerPos = world().dungeon()[level].randomPosition(randomEngine(), [this, level](sf::Vector2i pos, const Level&) {
		return world().isFree(make3D(pos, level));
	});
	position(make3D(playerPos, level));
}

void Player::handleEvent(sf::Event event) {
	if (state != State::WAITING_INPUT)
		return;

	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Numpad5)
			endTurn();
		else if (event.key.code == sf::Keyboard::Comma) {
			if (event.key.shift)
				tryAscentStairs();
		} else if (event.key.code == sf::Keyboard::Period) {
			if (event.key.shift)
				tryDescentStairs();
			return;
		} else if (isNumpad(event.key.code))
			for (int i = 1; i <= 9; ++i)
				if (sf::Keyboard::isKeyPressed(numpad(i))) {
					tryMove(numpadDirections<int>[i - 1], true);
					return;
				}
	}
}

void Player::DrawMemento::draw(Renderer& renderer) const {
	renderer.draw(*this);
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
		tryMoveTo(*newPos, true);
}

void Player::tryDescentStairs() {
	if (std::optional<sf::Vector3i> newPos = world().dungeon().downStairs(position()))
		tryMoveTo(*newPos, true);
}
