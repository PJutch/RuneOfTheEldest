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

void World::generate() {
	generationLogger->info("Started");
	levels.resize(10);
	for (int i = 0; i < levels.size(); ++ i) {
		generationLogger->info("Generating level {}...", i);

		levels[i].generateBlank({50, 50});

		generationLogger->info("Generating dungeon...");
		generator(levels[i]);

		generationLogger->info("Generating walls...");
		levels[i].generateWalls();
	}
	generationLogger->info("Finished");
}
