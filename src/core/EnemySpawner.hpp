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

#ifndef ENEMY_SPAWNER_HPP_
#define ENEMY_SPAWNER_HPP_

#include "World.hpp"
#include "Actor.hpp"

#include "render/AssetManager.hpp"
#include "render/PlayerMap.hpp"

#include "util/random.hpp"
#include "util/Map.hpp"

namespace core {
	class EnemySpawner {
	public:
		EnemySpawner(std::shared_ptr<World> world, std::shared_ptr<render::PlayerMap> playerMap, 
			         std::shared_ptr<render::AssetManager> assets, util::RandomEngine& randomEngine);

		void spawn();
	private:
		std::shared_ptr<World> world;
		std::shared_ptr<render::PlayerMap> playerMap;
		util::RandomEngine* randomEngine;

		struct EnemyData {
			Actor::Stats stats;

			int minOnLevel;
			int maxOnLevel;
		};
		std::vector<EnemyData> enemyData;
		Actor::Stats playerData;
	};
}

#endif