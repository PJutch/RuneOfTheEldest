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

#include "Event.hpp"
#include "World.hpp"

#include <SFML/System.hpp>

class Player {
public:
	Player(std::shared_ptr<World> world_) : 
		world{ std::move(world_) } {}

	sf::Vector2i position() const noexcept {
		return position_;
	}

	void position(sf::Vector2i newPosition) noexcept {
		position_ = newPosition;
	}

	int level() const noexcept {
		return level_;
	}

	void level(int newLevel) noexcept {
		level_ = newLevel;
	}

	void handleEvent(sf::Event event);
private:
	sf::Vector2i position_;
	int level_;

	std::shared_ptr<World> world;

	void tryMoveTo(sf::Vector2i newPosition) {
		if (isPassable(world->at(newPosition, level())))
			position(newPosition);
	}

	void tryMove(sf::Vector2i offset) {
		tryMoveTo(position() + offset);
	}
};

#endif
