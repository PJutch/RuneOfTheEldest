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

#include "AliveActor.hpp"

AliveActor::AliveActor(double newMaxHp, double regen_, sf::Vector3i newPosition, std::shared_ptr<World> newWorld) :
	position_{ newPosition }, hp_{ newMaxHp }, maxHp_{ newMaxHp }, regen{ regen_ }, world_{ std::move(newWorld) } {}

AliveActor::AliveActor(double newMaxHp, double regen_, std::shared_ptr<World> newWorld) :
	AliveActor{ newMaxHp, regen_, {0, 0, 0}, std::move(newWorld) } {}

void AliveActor::wait(int time) noexcept {
	nextTurn_ += time;

	hp_ += regen * time;
	hp(std::min(hp(), maxHp()));
}

void AliveActor::tryMoveTo(sf::Vector3i newPosition) {
	if (isPassable(world().dungeon().at(newPosition))) {
		if (auto other = world().actorAt(newPosition)) {
			if (other->isOnPlayerSide() == isOnPlayerSide()) {
				sf::Vector3i oldPosition = position();
				position(other->position());
				other->position(oldPosition);

				moveSucceed();
			} else
				attack(*other);
		} else {
			position(newPosition);
			moveSucceed();
		}
	}
}
