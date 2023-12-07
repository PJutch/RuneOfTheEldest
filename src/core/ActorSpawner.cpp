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

#include "render/AssetManager.hpp"

#include "util/parse.hpp"
#include "util/filesystem.hpp"
#include "util/Map.hpp"
#include <util/case.hpp>

#include <boost/describe.hpp>
#include <boost/mp11.hpp>

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

		class EffectNotFound : public LoadError {
		public:
			EffectNotFound(std::string_view name, util::Stacktrace currentStacktrace = {}) noexcept :
				LoadError{std::format("Effect \"{}\" not found", name), std::move(currentStacktrace)} {}
		};

		class SpellNotFound : public LoadError {
		public:
			SpellNotFound(std::string_view name, util::Stacktrace currentStacktrace = {}) noexcept :
				LoadError{std::format("Spell \"{}\" not found", name), std::move(currentStacktrace)} {}
		};
	}

	ActorSpawner::ActorSpawner(std::shared_ptr<World> world_, std::shared_ptr<XpManager> xpManager_, 
		                       std::shared_ptr<EffectManager> effectManager, std::shared_ptr<SpellManager> spellManager,
		                       render::Context renderContext_, util::LoggerFactory& loggerFactory,
		                       util::RandomEngine& randomEngine_,
							   std::shared_ptr<util::Raycaster> raycaster_) :
			world{ std::move(world_) }, xpManager{ std::move(xpManager_) }, 
			renderContext{std::move(renderContext_)},
			raycaster{ std::move(raycaster_) },
		    randomEngine{ &randomEngine_ }, logger{loggerFactory.create("actors")} {
		logger->info("Loading...");
		util::forEachFile("resources/Actors/", [&, this](std::ifstream& file, const std::filesystem::path& path) {
			logger->info("Loading spec from {} ...", path.generic_string());

			auto params = util::parseMapping(file);

			actorData.emplace_back();

			Actor::Stats& currentStats = actorData.back().stats;

			currentStats.maxHp = util::parseReal(util::getAndEraseRequired(params, "hp"));
			currentStats.regen = util::parseReal(util::getAndEraseRequired(params, "regen"));

			if (auto v = util::getAndErase(params, "mana"))
				currentStats.maxMana = util::parseReal(*v);
			if (auto v = util::getAndErase(params, "manaRegen"))
				currentStats.manaRegen = util::parseReal(*v);

			currentStats.damage = util::parseReal(util::getAndEraseRequired(params, "damage"));
			currentStats.accuracy = util::parseReal(util::getAndEraseRequired(params, "accuracy"));
			currentStats.evasion = util::parseReal(util::getAndEraseRequired(params, "evasion"));
			currentStats.turnDelay = util::parseReal(util::getAndEraseRequired(params, "turnDelay"));

			currentStats.xp = util::parseReal(util::getAndEraseRequired(params, "xp"));
			if (auto v = util::getAndErase(params, "hasRangedAttack")) {
				currentStats.hasRangedAttack = util::parseBool(*v);

				currentStats.projectileFlightTime = sf::seconds(util::parseReal(util::getAndEraseRequired(params, "projectileFlightTime")));
				currentStats.projectileTexture = &renderContext.assets->texture(util::getAndEraseRequired(params, "projectileTexture"));
			} else
				currentStats.hasRangedAttack = false;

			currentStats.defences.fill(1);
			boost::mp11::mp_for_each<boost::describe::describe_enumerators<DamageType>>([&](auto D) {
				using namespace std::literals;
				if (auto v = util::getAndErase(params, util::toLower(D.name) + "Defence"s))
					currentStats.defences[static_cast<int>(D.value)] = util::parseReal(*v);
			});

			if (auto v = util::getAndErase(params, "controller"))
				actorData.back().controller = *v;

			currentStats.texture = &renderContext.assets->texture(util::getAndEraseRequired(params, "texture"));
			actorData.back().minOnLevel = util::parseUint(util::getAndEraseRequired(params, "minOnLevel"));
			actorData.back().maxOnLevel = util::parseUint(util::getAndEraseRequired(params, "maxOnLevel"));

			if (auto v = util::getAndErase(params, "minLevel"))
				actorData.back().minLevel = util::parseUint(*v);
			if (auto v = util::getAndErase(params, "maxLevel"))
				actorData.back().maxLevel = util::parseUint(*v);

			if (auto v = util::getAndErase(params, "effect"))
				if (auto effect = effectManager->findEffect(*v))
					actorData.back().effectToAdd = effect;
				else
					throw EffectNotFound{*v};

			if (auto v = util::getAndErase(params, "spells"))
				for (auto spellName : util::parseList(*v) | std::views::transform(util::strip))
					if (auto spell = spellManager->findSpell(spellName))
						actorData.back().spellsToAdd.push_back(std::move(spell));
					else
						throw SpellNotFound{spellName};

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
			return std::make_unique<PlayerController>(actor, raycaster, renderContext);
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
					int count = std::uniform_int_distribution{data.minOnLevel, data.maxOnLevel}(*randomEngine);
					for (int i = 0; i < count; ++i)
						if (auto position = world->randomPositionAt(level, 1000, &World::isFree)) {
							auto enemy = std::make_shared<Actor>(data.stats, *position, world, xpManager,
								renderContext.particles, randomEngine);
							enemy->controller(createController(enemy, data.controller));

							if (data.effectToAdd)
								enemy->addEffect(data.effectToAdd->clone());
							for (const auto& spell : data.spellsToAdd)
								enemy->addSpell(spell->clone());

							world->addActor(std::move(enemy));
						}
				}
		logger->info("Spawned");
	}
}
