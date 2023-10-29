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

#ifndef ACTOR_SPAWNER_HPP_
#define ACTOR_SPAWNER_HPP_

#include "World.hpp"
#include "render/AssetManager.hpp"
#include "render/PlayerMap.hpp"

#include "util/random.hpp"
#include "util/Map.hpp"

#include <optional>

namespace core {
	class ActorSpawner {
	public:
		ActorSpawner(std::shared_ptr<World> world, std::shared_ptr<XpManager> xpManager, 
			         std::shared_ptr<EffectManager> effectManager,
			         std::shared_ptr<render::PlayerMap> playerMap,
					 std::shared_ptr<render::ParticleManager> particles,
			         std::shared_ptr<render::AssetManager> assets, util::LoggerFactory& loggerFactory, 
			         util::RandomEngine& randomEngine,
			         std::shared_ptr<util::Raycaster> raycaster);

		void spawn();
	private:
		std::shared_ptr<World> world;
		std::shared_ptr<XpManager> xpManager;
		std::shared_ptr<render::PlayerMap> playerMap;
		std::shared_ptr<render::ParticleManager> particles;
		std::shared_ptr<util::Raycaster> raycaster;
		util::RandomEngine* randomEngine;

		std::shared_ptr<spdlog::logger> logger;

		struct ActorData {
			Actor::Stats stats{};

			std::string controller = "enemy";

			int minOnLevel = 0;
			int maxOnLevel = 0;

			int minLevel = 0;
			std::optional<int> maxLevel;

			const Effect* effectToAdd = nullptr;
		};
		std::vector<ActorData> actorData;

		std::unique_ptr<Controller> createController(std::shared_ptr<Actor> actor, std::string_view type);
	};
}

#endif