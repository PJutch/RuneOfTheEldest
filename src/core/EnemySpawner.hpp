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
#include "Player.hpp"

#include "render/AssetManager.hpp"

#include "util/random.hpp"
#include "util/Map.hpp"

namespace core {
	class EnemySpawner {
	public:
		EnemySpawner(std::shared_ptr<World> world_, std::shared_ptr<Player> player_,
			         std::shared_ptr<render::AssetManager> assets_, util::RandomEngine& randomEngine_);

		void spawn();
	private:
		std::shared_ptr<World> world;
		std::shared_ptr<Player> player;
		util::RandomEngine* randomEngine;

		struct EnemyData {
			double hp;
			double regen;
			double damage;
			int turnDelay;

			const sf::Texture* texture;

			int minOnLevel;
			int maxOnLevel;
		};
		std::vector<EnemyData> enemyData;
	};
}

#endif