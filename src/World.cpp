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

void World::generate(std::shared_ptr<spdlog::logger> logger) {
    levels.resize(10);
	for (int i = 0; i < levels.size(); ++ i) {
		logger->info("Generating level {}...", i);

		levels[i].generateBlank({50, 50});

		logger->info("Generating dungeon...");
		generator(levels[i]);

		logger->info("Generating walls...");
		levels[i].generateWalls();

		if (i != 0) {
			logger->info("Generating stairs...");
			generateUpStairs(i, logger);
		}
	}
}

void World::addStairs(sf::Vector3i pos1, sf::Vector3i pos2) {
	if (pos1.z > pos2.z) {
		addStairs(pos2, pos1);
		return;
	}

	downStairs_.insert_or_assign(pos1, pos2);
	at(pos1) = Tile::DOWN_STAIRS;

	upStairs_.insert_or_assign(pos2, pos1);
	at(pos2) = Tile::UP_STAIRS;
}

void World::generateUpStairs(int fromLevel, std::shared_ptr<spdlog::logger> logger) {
	if (fromLevel <= 0)
		return;

	logger->info("Generating stairs...");
	for (int i = 0; i < 3; ++i) {
		sf::Vector2i upPos = (*this)[fromLevel - 1].randomPosition(*randomEngine, &isEmpty);

		sf::Vector2i downPos = (*this)[fromLevel].randomPosition(*randomEngine, &isEmpty);

		addStairs({ upPos.x, upPos.y, fromLevel - 1 }, { downPos.x, downPos.y, fromLevel });
	}
}
