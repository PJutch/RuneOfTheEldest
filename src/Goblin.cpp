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

#include "Goblin.hpp"

#include "World.hpp"

bool Goblin::act() {
	std::array<sf::Vector2i, 4> offsets{ sf::Vector2i{1, 0}, sf::Vector2i{-1, 0}, sf::Vector2i{0, 1}, sf::Vector2i{0, -1} };
	int offsetIndex = std::uniform_int_distribution<int>{ 0, std::ssize(offsets) - 1 }(*randomEngine);
	sf::Vector2i offset = offsets[offsetIndex];
	tryMove(offset);

	delayNextTurn(1);
	return true;
}

void Goblin::tryMoveTo(sf::Vector3i newPosition) {
	if (world->isPassable(newPosition)) {
		position_ = newPosition;
		delayNextTurn(1);
	}
}
