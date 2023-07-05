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

#include "util/parse.hpp"
#include "util/filesystem.hpp"
#include "util/Map.hpp"

namespace core {
	std::string EnemySpawner::unknownParamsMessage(std::unordered_map<std::string, std::string> params) {
		std::string message = "Unknown params: ";
		for (auto [name, value] : params)
			message += std::format("\"{}\" ", name);
		return message;
	}

	EnemySpawner::EnemySpawner(std::shared_ptr<World> world_, std::shared_ptr<Player> player_,
							   std::shared_ptr<render::AssetManager> assets_, util::RandomEngine& randomEngine_) :
			world{ std::move(world_) }, player{ std::move(player_) }, assets{ std::move(assets_) }, randomEngine{ &randomEngine_ } {
		util::forEachFile("resources/enemies/", [this](std::ifstream& file) {
			auto params = util::parseMapping(file);
			
			enemyData.emplace_back();
			processParam(params, "hp", [this](std::string_view value) {
				enemyData.back().hp = util::parseReal<double>(value);
			});
			processParam(params, "regen", [this](std::string_view value) {
				enemyData.back().regen = util::parseReal<double>(value);
			});
			processParam(params, "texture", [this](std::string_view value) {
				enemyData.back().texture = &assets->texture(value);
			});
			processParam(params, "minOnLevel", [this](std::string_view value) {
				enemyData.back().minOnLevel = util::parseInt<int>(value);
			});
			processParam(params, "maxOnLevel", [this](std::string_view value) {
				enemyData.back().maxOnLevel = util::parseInt<int>(value);
			});

			if (!params.empty())
				throw UnknownParamsError{ params };
		});
	}

	void EnemySpawner::spawn() {
		for (int level = 0; level < world->tiles().shape().z; ++level)
			for (const EnemyData& enemyData : enemyData)
				for (int i = 0; i < std::uniform_int_distribution{ enemyData.minOnLevel, enemyData.maxOnLevel }(*randomEngine); ++i)
					Enemy::spawnSingle(level, enemyData.hp, enemyData.regen, *enemyData.texture, world, player, *randomEngine);
	}
}
