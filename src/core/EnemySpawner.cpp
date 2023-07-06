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
	namespace {
		class LoadError : public util::RuntimeError {
		public:
			LoadError(const std::string& message, util::Stacktrace currentStacktrace = {}) noexcept :
				RuntimeError{ message, std::move(currentStacktrace) } {}
		};

		class NoValueError : public LoadError {
		public:
			NoValueError(std::string_view name, util::Stacktrace currentStacktrace = {}) noexcept :
				LoadError{ std::format("Value for required param \"{}\" is not given", name), std::move(currentStacktrace) } {}
		};

		std::string unknownParamsMessage(std::unordered_map<std::string, std::string> params) {
			std::string message = "Unknown params: ";
			for (auto [name, value] : params)
				message += std::format("\"{}\" ", name);
			return message;
		}

		class UnknownParamsError : public LoadError {
		public:
			UnknownParamsError(std::unordered_map<std::string, std::string> params, util::Stacktrace currentStacktrace = {}) noexcept :
				LoadError{ unknownParamsMessage(params), std::move(currentStacktrace) } {}
		};

		template <typename Callback>
		void processParam(std::unordered_map<std::string, std::string>& params, const std::string& name, Callback&& callback) {
			if (auto value = util::getAndErase(params, name))
				callback(*value);
			else
				throw NoValueError{ name };
		}
	}

	EnemySpawner::EnemySpawner(std::shared_ptr<World> world_, std::shared_ptr<Player> player_,
							   std::shared_ptr<render::AssetManager> assets, util::RandomEngine& randomEngine_) :
			world{ std::move(world_) }, player{ std::move(player_) }, randomEngine{ &randomEngine_ } {
		util::forEachFile("resources/enemies/", [this, &assets](std::ifstream& file) {
			auto params = util::parseMapping(file);
			
			enemyData.emplace_back();
			processParam(params, "hp", [this](std::string_view value) {
				enemyData.back().stats.maxHp = util::parseReal(value);
			});
			processParam(params, "regen", [this](std::string_view value) {
				enemyData.back().stats.regen = util::parseReal(value);
			});
			processParam(params, "damage", [this](std::string_view value) {
				enemyData.back().stats.damage = util::parseReal(value);
			});
			processParam(params, "turnDelay", [this](std::string_view value) {
				enemyData.back().stats.turnDelay = util::parseUint(value);
			});
			processParam(params, "texture", [this, &assets](std::string_view value) {
				enemyData.back().stats.texture = &assets->texture(value);
			});
			processParam(params, "minOnLevel", [this](std::string_view value) {
				enemyData.back().minOnLevel = util::parseUint(value);
			});
			processParam(params, "maxOnLevel", [this](std::string_view value) {
				enemyData.back().maxOnLevel = util::parseUint(value);
			});

			if (!params.empty())
				throw UnknownParamsError{ params };
		});
	}

	void EnemySpawner::spawn() {
		for (int level = 0; level < world->tiles().shape().z; ++level)
			for (const EnemyData& enemyData : enemyData)
				for (int i = 0; i < std::uniform_int_distribution{ enemyData.minOnLevel, enemyData.maxOnLevel }(*randomEngine); ++i)
					Enemy::spawnSingle(level, enemyData.stats, world, player, *randomEngine);
	}
}
