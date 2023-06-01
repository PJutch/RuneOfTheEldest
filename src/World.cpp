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

#include "Goblin.hpp"

World::World(std::shared_ptr<Dungeon> newDungeon_,
	         std::shared_ptr<Player> newPlayer,
	         RandomEngine& randomEngine_,
	         LoggerFactory& loggerFactory) :
		dungeon_{ std::move(newDungeon_) },
	    player_{ std::move(newPlayer) },
		generationLogger{ loggerFactory.create("generation") },
		randomEngine{ &randomEngine_ } {
	actors_.push_back(player_);
	push_actor();
}

/// @brief Generates dungeon and places player
/// @param logger logger to log messages
void World::generate() {
	player_->world(shared_from_this());

	generationLogger->info("Started");
	dungeon().generate(generationLogger);


	generationLogger->info("Placing player...");
	int playerLevel = 0;
	sf::Vector2i playerPos = dungeon()[playerLevel].randomPosition(*randomEngine, &isPassableTile);
	player_->position(make3D(playerPos, playerLevel));

	generationLogger->info("Spawning goblins...");
	for (int i = 0; i < dungeon().size(); ++i)
		spawnGoblins(i);

	generationLogger->info("Finished");
}

void World::spawnGoblins(int levelIndex) {
	for (int i = 0; i < std::uniform_int_distribution{ 5, 20 }(*randomEngine); ++i) {
		sf::Vector2i position = dungeon()[levelIndex].randomPosition(*randomEngine, [this, levelIndex](sf::Vector2i pos, const Level& level) {
			return isPassable(make3D(pos, levelIndex));
			});

		actors_.push_back(make_shared<Goblin>(make3D(position, levelIndex), shared_from_this(), *randomEngine));
		push_actor();
	}
}
