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
#include "util/Direction.hpp"

namespace render {
	PlayerMap::PlayerMap(std::shared_ptr<core::World> world_, std::shared_ptr<util::Raycaster> raycaster_) :
		world{ std::move(world_) }, raycaster{std::move(raycaster_)} {}

	const bool seeEverything = true;

	void PlayerMap::onGenerate() {
		seenActors_.clear();
		clearSounds();

		tileStates.assign(world->tiles().shape(), seeEverything ? TileState::VISIBLE : TileState::UNSEEN);
	}

	bool PlayerMap::canSee(sf::Vector3i position) const noexcept {
		return seeEverything || raycaster->canSee(world->player().position(), position);
	}

	void PlayerMap::updateTiles() {
		if (seeEverything)
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
			return seeEverything || raycaster->canSee(player.position(), actor.position);
		});

		for (const auto& actor : world->actors())
			if (actor->isAlive() && (seeEverything || raycaster->canSee(world->player().position(), actor->position())))
				seenActors_.emplace_back(actor->position(), 
										 actor->hp(), actor->maxHp(), 
										 actor->mana(), actor->maxMana(),
					                     actor->controller().aiState(), actor->texture());

		std::ranges::sort(seenActors_, {}, [](SeenActor actor) {
			return actor.position.y;
		});
	}

	void PlayerMap::discoverLevelTiles(int z) {
		if (seeEverything)
			return;

		auto [shapeX, shapeY, shapeZ] = world->tiles().shape();
		for (int x = 0; x < shapeX; ++x)
			for (int y = 0; y < shapeY; ++y) 
				if (tileStates[{ x, y, z }] == TileState::UNSEEN) {
					if (std::ranges::any_of(util::directions<int>, [&](sf::Vector2i d) {
						sf::Vector3i pos{x + d.x, y + d.y, z};
						return world->tiles().isValidPosition(pos) && world->tiles()[pos] != core::Tile::WALL;
					})) {
						tileStates[{ x, y, z }] = TileState::MEMORIZED;
					}
				}
	}

	void PlayerMap::discoverLevelActors(int z) {
		if (seeEverything)
			return;

		std::erase_if(seenActors_, [z](auto actor) -> bool {
			return actor.position.z == z;
		});

		for (const auto& actor : world->actors())
			if (actor->isAlive() && actor->position().z == z)
				seenActors_.emplace_back(actor->position(),
					actor->hp(), actor->maxHp(),
					actor->mana(), actor->maxMana(),
					actor->controller().aiState(), actor->texture());

		std::ranges::sort(seenActors_, {}, [](SeenActor actor) {
			return actor.position.y;
		});
	}
}
