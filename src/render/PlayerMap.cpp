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
	PlayerMap::PlayerMap(std::shared_ptr<core::World> world_, std::shared_ptr<util::Raycaster> raycaster_) :
		world{ std::move(world_) }, raycaster{std::move(raycaster_)} {}

	void PlayerMap::onGenerate() {
		seenActors_.clear();
		clearSounds();

		tileStates.assign(world->tiles().shape(), seeEverything_ ? TileState::VISIBLE : TileState::UNSEEN);
	}

	void PlayerMap::updateTiles() {
		if (seeEverything_)
			return;

		auto [shapeX, shapeY, shapeZ] = world->tiles().shape();
		for (int z = 0; z < shapeZ; ++z) {
			for (int x = 0; x < shapeX; ++x)
				for (int y = 0; y < shapeY; ++y)
					if (raycaster->canSee(world->player().position(), { x, y, z }))
						tileStates[{ x, y, z }] = TileState::VISIBLE;
					else if (tileState({ x, y, z }) == TileState::VISIBLE)
						tileStates[{ x, y, z }] = TileState::MEMORIZED;
		}
	}

	void PlayerMap::updateActors() {
		std::erase_if(seenActors_, [this, &player = world->player(), &world = *world](auto actor) -> bool {
			return seeEverything_ || raycaster->canSee(player.position(), actor.position);
		});

		for (const auto& actor : world->actors())
			if (actor->isAlive() && (seeEverything_ || raycaster->canSee(world->player().position(), actor->position())))
				seenActors_.emplace_back(actor->position(), actor->hp(), actor->maxHp(), 
					                     actor->controller().aiState(), actor->texture() );
	}
}
