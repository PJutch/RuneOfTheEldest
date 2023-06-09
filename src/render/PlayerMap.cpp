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

#include "core/World.hpp"

#include "util/raycast.hpp"

namespace render {
	PlayerMap::PlayerMap(std::shared_ptr<core::World> world_) : world{ std::move(world_) } {}

	void PlayerMap::onGenerate() {
		seenActors_.clear();
		tileStates.assign(world->tiles().shape(), TileState::UNSEEN);
	}

	void PlayerMap::updateTiles() {
		auto [shapeX, shapeY, shapeZ] = world->tiles().shape();
		for (int z = 0; z < shapeZ; ++z) {
			for (int x = 0; x < shapeX; ++x)
				for (int y = 0; y < shapeY; ++y)
					if (util::canSee(world->player().position(), { x, y, z }, *world))
						tileStates[{ x, y, z }] = TileState::VISIBLE;
					else if (tileState({ x, y, z }) == TileState::VISIBLE)
						tileStates[{ x, y, z }] = TileState::MEMORIZED;
		}
	}

	void PlayerMap::updateActors() {
		std::erase_if(seenActors_, [&player = world->player(), &world = *world](auto actor) -> bool {
			return util::canSee(player.position(), actor.position, world);
		});

		for (const auto& actor : world->actors())
			if (util::canSee(world->player().position(), actor->position(), *world))
				seenActors_.emplace_back(actor->position(), actor->hp(), actor->maxHp(), actor->controller().aiState(), actor->texture() );
	}
}
