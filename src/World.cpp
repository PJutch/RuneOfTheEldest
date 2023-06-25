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

std::shared_ptr<Actor> World::actorAt(sf::Vector3i position) {
	auto iter = std::ranges::find_if(actors_, [position](std::shared_ptr<Actor> actor) {
		return actor->isAlive() && actor->position() == position;
	});

	if (iter == actors_.end())
		return nullptr;
	return *iter;
}

std::shared_ptr<const Actor> World::actorAt(sf::Vector3i position) const {
	auto iter = std::ranges::find_if(actors_, [position](std::shared_ptr<Actor> actor) {
		return actor->isAlive() && actor->position() == position;
	});

	if (iter == actors_.end())
		return nullptr;
	return *iter;
}

void World::update() {
	while (!actors_.empty()) {
		popActor();

		if (actors_.back()->isAlive()) {
			bool complete = actors_.back()->act();
			pushActor();
			if (!complete)
				break;
		}
		else {
			bool interrupt = actors_.back()->shouldInterruptOnDelete();
			actors_.pop_back();
			if (interrupt)
				break;
		}
	}
}

void World::makeSound(Sound sound) {
	for (auto actor : actors())
		actor->handleSound(sound);
}

void World::generateStairs() {
	for (int z = 1; z < dungeon().shape().z; ++z)
		for (int i = 0; i < 3; ++i)
			addStairs(randomPositionAt(z - 1, &isEmpty), randomPositionAt(z, &isEmpty));
}

void World::addStairs(sf::Vector3i pos1, sf::Vector3i pos2) {
	if (pos1.z > pos2.z) {
		addStairs(pos2, pos1);
		return;
	}

	downStairs_.insert_or_assign(pos1, pos2);
	dungeon()[pos1] = Tile::DOWN_STAIRS;

	upStairs_.insert_or_assign(pos2, pos1);
	dungeon()[pos2] = Tile::UP_STAIRS;
}
