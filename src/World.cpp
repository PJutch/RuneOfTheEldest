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

World::World(std::shared_ptr<Dungeon> dungeon_,
	         std::shared_ptr<Player> player_,
	         RandomEngine& randomEngine_,
	         LoggerFactory& loggerFactory) :
	dungeon{ std::move(dungeon_) },
	player{ std::move(player_) },
	generationLogger{ loggerFactory.create("generation") },
	randomEngine{ &randomEngine_ } {}

/// @brief Generates dungeon and places player
/// @param logger logger to log messages
void World::generate() {
	generationLogger->info("Started");
	dungeon->generate(generationLogger);

	generationLogger->info("Placing player...");
	player->level(0);
	player->position((*dungeon)[player->level()].randomPosition(*randomEngine, &isPassable));

	generationLogger->info("Finished");
}
