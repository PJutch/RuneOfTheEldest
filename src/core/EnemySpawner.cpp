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

#include "EnemySpawner.hpp"

#include "Enemy.hpp"

namespace core {
	EnemySpawner::EnemySpawner(std::shared_ptr<World> world_, std::shared_ptr<Player> player_,
							   std::shared_ptr<render::AssetManager> assets_, util::RandomEngine& randomEngine_) :
			world{ std::move(world_) }, player{ std::move(player_) }, assets{ std::move(assets_) }, randomEngine{ &randomEngine_ } {
		enemyData.emplace_back(3.0, 0.1, &assets->texture("resources/textures/goblin.png"), 5, 20);
	}

	void EnemySpawner::spawn() {
		for (int level = 0; level < world->tiles().shape().z; ++level)
			for (const EnemyData& enemyData : enemyData)
				for (int i = 0; i < std::uniform_int_distribution{ enemyData.minOnLevel, enemyData.maxOnLevel }(*randomEngine); ++i)
					Enemy::spawnSingle(level, enemyData.hp, enemyData.regen, *enemyData.texture, world, player, *randomEngine);
	}
}
