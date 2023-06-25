/* This file is part of the Rune of the Eldest.
The Rune of the Eldest - Roguelike about the mage seeking for ancient knowledges
Copyright(C) 2023  PJutch

The Rune of the Eldest is free software : you can redistribute it and /or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

The Rune of the Eldest is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with the Rune of the Eldest.
If not, see < https://www.gnu.org/licenses/>. */

#include "PlayerMap.hpp"

#include "Player.hpp"
#include "World.hpp"

#include "raycast.hpp"

PlayerMap::PlayerMap(std::shared_ptr<Player> player_, std::shared_ptr<World> world_) :
	player{ std::move(player_) }, world{ std::move(world_) } {}

void PlayerMap::onGenerate() {
	seenActors_.clear();
	tileStates.assign(world->dungeon().shape());
}

void PlayerMap::updateTiles() {
	auto [shapeX, shapeY, shapeZ] = world->dungeon().shape();
	for (int z = 0; z < shapeZ; ++z) {
		for (int x = 0; x < shapeX; ++x)
			for (int y = 0; y < shapeY; ++y)
				if (canSee(player->position(), { x, y, z }, world->dungeon()))
					tileStates[{ x, y, z }] = TileState::VISIBLE;
				else if (tileState({ x, y, z }) == TileState::VISIBLE)
					tileStates[{ x, y, z }] = TileState::MEMORIZED;
	}
}

void PlayerMap::updateActors() {
	std::erase_if(seenActors_, [&player = *player, &dungeon = world->dungeon()](const auto& actor) -> bool {
		return canSee(player.position(), actor->position(), dungeon);
	});

	for (const auto& actor : world->actors())
		if (canSee(player->position(), actor->position(), world->dungeon()))
			seenActors_.push_back(actor->createDrawMemento());
}
