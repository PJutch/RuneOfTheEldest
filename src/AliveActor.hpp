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

#ifndef ALIVE_ACTOR_HPP_
#define ALIVE_ACTOR_HPP_

#include "Actor.hpp"

#include "World.hpp"

/// Base for all Actors with hp and position in world
class AliveActor : public Actor {
public:
	sf::Vector3i position() const noexcept final {
		return position_;
	}

	int nextTurn() const noexcept final {
		return nextTurn_;
	}

	bool isAlive() const final {
		return hp() > 0;
	}

	void beDamaged(double damage) final {
		hp_ -= damage;
	}

	/// Gets Actor HP
	double hp() const noexcept {
		return hp_;
	}

	/// Gets max possible HP
	double maxHp() const noexcept {
		return maxHp_;
	}
protected:
	AliveActor(double newMaxHp, double regen_, sf::Vector3i newPosition, std::shared_ptr<World> newWorld) : 
		position_{ newPosition }, hp_{ newMaxHp }, maxHp_{ newMaxHp }, regen{ regen_ }, world_ {std::move(newWorld)} {}
	AliveActor(double newMaxHp, double regen_, std::shared_ptr<World> newWorld) : 
		AliveActor{ newMaxHp, regen_, {0, 0, 0}, std::move(newWorld) } {}

	World& world() noexcept {
		return *world_;
	}

	const World& world() const noexcept {
		return *world_;
	}

	/// Sets next turn time
	void nextTurn(int newNextTurn) noexcept {
		nextTurn_ = newNextTurn;
	}

	/// Dealys next Actor turn and applies over time effects
	void wait(int time) noexcept {
		nextTurn_ += time;

		hp_ += regen * time;
		hp(std::min(hp(), maxHp()));
	}

	/// Attacks other Actor
	virtual void attack(Actor& actor) = 0;

	/// Callled after successful move
	virtual void moveSucceed() {}

	/// Sets position in world
	void position(sf::Vector3i newPosition) noexcept {
		position_ = newPosition;
	}

	/// changes position if newPosition isn't occupied
	void tryMoveTo(sf::Vector3i newPosition) {
		if (isPassable(world().dungeon().at(newPosition))) {
			if (auto other = world().actorAt(newPosition))
				attack(*other);
			else {
				position(newPosition);
				moveSucceed();
			}
		}
	}

	/// changes position if newPosition isn't occupied
	void tryMoveTo(sf::Vector2i newPosition) {
		tryMoveTo(make3D(newPosition, position().z));
	}

	/// changes position if position + offset isn't occupied
	void tryMove(sf::Vector2i offset) {
		tryMoveTo(getXY(position()) + offset);
	}

	void hp(double newHp) noexcept {
		hp_ = newHp;
	}
private:
	int nextTurn_ = 0;
	sf::Vector3i position_;

	double hp_;
	double maxHp_;
	double regen;

	std::shared_ptr<World> world_;
};

#endif
