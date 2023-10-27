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

#include "ActorSpawner.hpp"

#include "Controller/EnemyAi.hpp"
#include "Controller/PlayerController.hpp"

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

		std::string unknownParamsMessage(std::unordered_map<std::string, std::string> params) {
			std::string message = "Unknown params: ";
			for (auto [name, value] : params)
				message += std::format("\"{}\" ", name);
			return message;
		}

		class UnknownParamsError : public LoadError {
		public:
			UnknownParamsError(std::unordered_map<std::string, std::string> params,
				util::Stacktrace currentStacktrace = {}) noexcept :
				LoadError{ unknownParamsMessage(params), std::move(currentStacktrace) } {}
		};
	}

	ActorSpawner::ActorSpawner(std::shared_ptr<World> world_, std::shared_ptr<XpManager> xpManager_, 
		                       std::shared_ptr<EffectManager> effectManager,
		                       std::shared_ptr<render::PlayerMap> playerMap_,
							   std::shared_ptr<render::AssetManager> assets, util::LoggerFactory& loggerFactory, 
		                       util::RandomEngine& randomEngine_,
							   std::shared_ptr<util::Raycaster> raycaster_) :
			world{ std::move(world_) }, xpManager{ std::move(xpManager_) }, playerMap{std::move(playerMap_)},
			raycaster{ std::move(raycaster_) }, randomEngine{ &randomEngine_ }, logger{loggerFactory.create("actors")} {
		logger->info("Loading...");
		util::forEachFile("resources/Actors/", [&, this](std::ifstream& file, const std::filesystem::path& path) {
			logger->info("Loading spec from {} ...", path.generic_string());

			auto params = util::parseMapping(file);

			actorData.emplace_back();
			actorData.back().stats.maxHp = util::parseReal(util::getAndEraseRequired(params, "hp"));
			actorData.back().stats.regen = util::parseReal(util::getAndEraseRequired(params, "regen"));
			actorData.back().stats.damage = util::parseReal(util::getAndEraseRequired(params, "damage"));
			actorData.back().stats.accuracy = util::parseReal(util::getAndEraseRequired(params, "accuracy"));
			actorData.back().stats.evasion = util::parseReal(util::getAndEraseRequired(params, "evasion"));
			actorData.back().stats.turnDelay = util::parseReal(util::getAndEraseRequired(params, "turnDelay"));
			actorData.back().stats.xp = util::parseReal(util::getAndEraseRequired(params, "xp"));

			for (int i = 0; i < totalDamageTypes; ++i) {
				std::string paramName = damageTypeNames[i] + "Defence";
				actorData.back().stats.defences[i] = util::parseReal(util::getAndEraseRequired(params, paramName));
			}

			if (auto v = util::getAndErase(params, "controller"))
				actorData.back().controller = *v;

			actorData.back().stats.texture = &assets->texture(util::getAndEraseRequired(params, "texture"));
			actorData.back().minOnLevel = util::parseUint(util::getAndEraseRequired(params, "minOnLevel"));
			actorData.back().maxOnLevel = util::parseUint(util::getAndEraseRequired(params, "maxOnLevel"));

			if (auto v = util::getAndErase(params, "minLevel"))
				actorData.back().minLevel = util::parseUint(*v);
			if (auto v = util::getAndErase(params, "maxLevel"))
				actorData.back().maxLevel = util::parseUint(*v);

			if (auto v = util::getAndErase(params, "effect"))
				actorData.back().effectToAdd = effectManager->findEffect(*v);

			if (!params.empty())
				throw UnknownParamsError{ params };
		});
		logger->info("Loaded");
	}

	namespace {
		class UnknownControllerError : public util::RuntimeError {
		public:
			UnknownControllerError(std::string_view type, util::Stacktrace currentStacktrace = {}) noexcept :
				util::RuntimeError{ std::format("Unknow controller {}", type), std::move(currentStacktrace) } {}
		};
	}

	std::unique_ptr<Controller> ActorSpawner::createController(std::shared_ptr<Actor> actor, std::string_view type) {
		if (type == "player")
			return std::make_unique<PlayerController>(actor, playerMap);
		else if (type == "enemy")
			return std::make_unique<EnemyAi>(actor, raycaster);
		else
			throw UnknownControllerError{ type };
	}

	void ActorSpawner::spawn() {
		logger->info("Spawning...");
		for (int level = 0; level < world->tiles().shape().z; ++level)
			for (const ActorData& data : actorData) 
				if (data.minLevel <= level && (!data.maxLevel || level <= data.maxLevel)) {
					int count = std::uniform_int_distribution{ data.minOnLevel, data.maxOnLevel }(*randomEngine);
					for (int i = 0; i < count; ++i) {
						sf::Vector3i position = world->randomPositionAt(level, &World::isFree);
						auto enemy = std::make_shared<Actor>(data.stats, position, world, xpManager, randomEngine);
						enemy->controller(createController(enemy, data.controller));
						if (data.effectToAdd)
							enemy->addEffect(data.effectToAdd->clone());
						world->addActor(std::move(enemy));
					}
			}
		logger->info("Spawned");
	}
}
