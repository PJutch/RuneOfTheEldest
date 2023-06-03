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

#include "AliveActor.hpp"

#include "Renderer.hpp"

#include "random.hpp"
#include "geometry.hpp"

#include <SFML/System.hpp>

#include <array>
#include <memory>

/// Goblin enemy
class Goblin : public AliveActor {
public:
	Goblin(sf::Vector3i newPosition, std::shared_ptr<World> world_, RandomEngine& randomEngine_) :
		AliveActor{ 3, 0.1, newPosition, std::move(world_) }, randomEngine{ &randomEngine_ } {}

	/// Randomly moves goblin
	bool act() final;

	static void spawnSingle(int level, std::shared_ptr<World> world, RandomEngine& randomEngine);
	static void spawnAll(std::shared_ptr<World> world, RandomEngine& randomEngine);

	void draw(Renderer& renderer) const final {
		if (isAlive())
			renderer.draw(*this);
	}

	bool shouldInterruptOnDelete() const final {
		return false;
	}
private:
	RandomEngine* randomEngine;

	void attack(Actor& actor) final {
		actor.beDamaged(1);
	}
};

#endif