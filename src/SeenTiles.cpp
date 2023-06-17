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

#include "SeenTiles.hpp"

#include "Player.hpp"
#include "Dungeon.hpp"

#include "raycast.hpp"

SeenTiles::SeenTiles(std::shared_ptr<Player> player_, std::shared_ptr<Dungeon> dungeon_) :
	player{ std::move(player_) }, dungeon{ std::move(dungeon_) } {}

void SeenTiles::onGenerate() {
	tileStates.resize(dungeon->size());
	shapes.reserve(dungeon->size());

	for (int z = 0; z < dungeon->size(); ++z) {
		auto [shapeX, shapeY] = (*dungeon)[z].shape();
		tileStates[z].resize(shapeX * shapeY);
		shapes.emplace_back(shapeX, shapeY);
	}
}

void SeenTiles::update() {
	int z = player->position().z;
	const Level& level = (*dungeon)[z];
	for (int x = 0; x < level.shape().x; ++x)
		for (int y = 0; y < level.shape().y; ++y)
			if (canSee(player->position(), { x, y, z }, *dungeon))
				tileStateMut({ x, y, z }) = TileState::VISIBLE;
			else if (tileState({ x, y, z }) == TileState::VISIBLE)
				tileStateMut({ x, y, z }) = TileState::MEMORIZED;
}
