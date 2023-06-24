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
	tileStates.clear();
	shapes.clear();
	seenActors_.clear();

	tileStates.resize(world->dungeon().size());
	shapes.reserve(world->dungeon().size());

	for (int z = 0; z < world->dungeon().size(); ++z) {
		auto [shapeX, shapeY] = world->dungeon()[z].shape();
		tileStates[z].resize(shapeX * shapeY);
		shapes.emplace_back(shapeX, shapeY);
	}
}

void PlayerMap::updateTiles() {
	for (int z = 0; z < world->dungeon().size(); ++z) {
		const Level& level = world->dungeon()[z];
		for (int x = 0; x < level.shape().x; ++x)
			for (int y = 0; y < level.shape().y; ++y)
				if (canSee(player->position(), { x, y, z }, world->dungeon()))
					tileStateMut({ x, y, z }) = TileState::VISIBLE;
				else if (tileState({ x, y, z }) == TileState::VISIBLE)
					tileStateMut({ x, y, z }) = TileState::MEMORIZED;
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
