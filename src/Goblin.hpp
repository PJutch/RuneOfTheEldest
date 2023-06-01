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

#ifndef GOBLIN_HPP_
#define GOBLIN_HPP_

#include "Actor.hpp"

#include "random.hpp"
#include "geometry.hpp"

#include <SFML/System.hpp>

#include <array>
#include <memory>

class World;

/// Goblin enemy
class Goblin : public Actor {
public:
	Goblin(sf::Vector3i newPosition, std::shared_ptr<World> world_, RandomEngine& randomEngine_) :
		position_{ newPosition }, world {std::move(world_)}, randomEngine{ &randomEngine_ } {}

	/// Position in world
	sf::Vector3i position() const noexcept {
		return position_;
	}

	sf::Vector3i position3() const noexcept {
		return position_;
	}

	bool act() final;
private:
	sf::Vector3i position_;

	std::shared_ptr<World> world;
	RandomEngine* randomEngine;

	void tryMoveTo(sf::Vector3i newPosition);

	void tryMoveTo(sf::Vector2i newPosition) {
		tryMoveTo(make3D(newPosition, position_.z));
	}

	void tryMove(sf::Vector2i offset) {
		tryMoveTo(getXY(position()) + offset);
	}

	void tryMove(sf::Vector3i offset) {
		tryMoveTo(position() + offset);
	}
};

#endif