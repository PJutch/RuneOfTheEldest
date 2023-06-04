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

#include "World.hpp"
#include "Actor.hpp"

#include <gtest/gtest.h>

class TestActor : public Actor {
public:
	TestActor(int firstTurn, int turnDelay_, sf::Vector3i newPosition) noexcept :
		nextTurn_{ firstTurn }, turnDelay{ turnDelay_ }, position_{ newPosition } {}

	sf::Vector3i position() const noexcept final {
		return position_;
	}

	bool act() override {
		nextTurn_ += turnDelay;
		return true;
	}

	int nextTurn() const noexcept final {
		return nextTurn_;
	}

	bool isAlive() const noexcept {
		return true;
	}

	bool shouldInterruptOnDelete() const noexcept {
		return false;
	}

	void beDamaged(double damage) noexcept final {}

	void draw(Renderer& renderer) const noexcept final {}
private:
	int nextTurn_;
	int turnDelay;
	sf::Vector3i position_;
};

class LoggingTestActor : public TestActor {
public:
	LoggingTestActor(int firstTurn, int turnDelay_, sf::Vector3i newPosition, int id_, std::vector<int>& log_) noexcept :
		TestActor{ firstTurn, turnDelay_, newPosition }, id{ id_ }, log{ &log_ } {}

	bool act() {
		log->push_back(id);
		return TestActor::act();
	}
private:
	int id;
	std::vector<int>* log;
};

TEST(World, addActor) {
	World world{ nullptr };
}
