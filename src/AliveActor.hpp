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

#ifndef ENTITY_HPP_
#define ENTITY_HPP_

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

	void beDamaged(int damage) final {
		hp_ -= damage;
	}

	/// Gets Actor HP
	int hp() const noexcept {
		return hp_;
	}

	/// Gets max possible HP
	int maxHp() const noexcept {
		return maxHp_;
	}
protected:
	AliveActor(int newMaxHp, sf::Vector3i newPosition, std::shared_ptr<World> newWorld) : 
		position_{ newPosition }, hp_{ newMaxHp }, maxHp_{ newMaxHp }, world_{ std::move(newWorld) } {}
	AliveActor(int newMaxHp, std::shared_ptr<World> newWorld) : AliveActor{ newMaxHp, {0, 0, 0}, std::move(newWorld) } {}

	World& world() noexcept {
		return *world_;
	}

	const World& world() const noexcept {
		return *world_;
	}

	/// Add delay units to time before next turn
	void delayNextTurn(int delay) noexcept {
		nextTurn_ += delay;
	}

	/// Attacks other Actor
	virtual void attack(Actor& actor) = 0;

	/// Callled once at the end of the turn
	virtual void endTurn() = 0;

	/// Sets position in world
	void position(sf::Vector3i newPosition) noexcept {
		position_ = newPosition;
	}

	/// changes position if newPosition isn't occupied
	void tryMoveTo(sf::Vector3i newPosition) {
		if (isPassable(world().dungeon().at(newPosition))) {
			if (auto other = world().actorAt(newPosition))
				attack(*other);
			else
				position(newPosition);
			endTurn();
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
private:
	int nextTurn_ = 0;
	sf::Vector3i position_;

	int hp_;
	int maxHp_;

	std::shared_ptr<World> world_;
};

#endif
