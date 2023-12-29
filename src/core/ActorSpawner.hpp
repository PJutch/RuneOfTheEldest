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
#include "Spell/Manager.hpp"
#include "Effect/Effect.hpp"
#include "ItemManager.hpp"
#include "Actor.hpp"

#include "render/Context.hpp"

#include "util/random.hpp"
#include "util/Map.hpp"
#include "util/raycast.hpp"
#include "util/pathfinding.hpp"

#include <optional>

namespace core {
	class ActorSpawner {
	public:
		ActorSpawner(std::shared_ptr<World> world, std::shared_ptr<XpManager> xpManager, 
			         std::shared_ptr<EffectManager> effectManager, std::shared_ptr<SpellManager> spellManager,
					 std::shared_ptr<ItemManager> itemManager_,
			         render::Context renderContext, util::LoggerFactory& loggerFactory,
			         util::RandomEngine& randomEngine,
			         std::shared_ptr<util::Raycaster> raycaster);

		void spawn();

		std::shared_ptr<core::Actor> parseActor(std::string_view s);
		std::string stringifyActor(const core::Actor& actor) const;

		void onSaveLoaded();
	private:
		std::shared_ptr<World> world;
		std::shared_ptr<XpManager> xpManager;
		std::shared_ptr<EffectManager> effectManager;
		std::shared_ptr<SpellManager> spellManager;
		std::shared_ptr<ItemManager> itemManager;
		render::Context renderContext;
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
			std::vector<std::shared_ptr<Spell>> spellsToAdd;
		};
		std::vector<ActorData> actorData;

		struct SpellToAdd {
			std::shared_ptr<Actor> actor;
			std::string spellData;
		};
		std::vector<SpellToAdd> spellsToAdd;

		std::unique_ptr<Controller> createController(std::shared_ptr<Actor> actor, std::string_view type) const;
	};
}

#endif