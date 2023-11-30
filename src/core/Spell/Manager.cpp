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

#include "Manager.hpp"
#include "Projectile.hpp"
#include "FallingProjectile.hpp"
#include "BranchingRay.hpp"
#include "ChargingRay.hpp"

#include "core/DamageType.hpp"

#include "render/AssetManager.hpp"

#include "util/parse.hpp"
#include "util/Map.hpp"
#include "util/filesystem.hpp"

#include <memory>

namespace core {
	namespace {
		std::string unknownParamsMessage(std::unordered_map<std::string, std::string> params) {
			std::string message = "Unknown params: ";
			for (auto [name, value] : params)
				message += std::format("\"{}\" ", name);
			return message;
		}

		class UnknownParamsError : public util::RuntimeError {
		public:
			UnknownParamsError(std::unordered_map<std::string, std::string> params,
				util::Stacktrace currentStacktrace = {}) noexcept :
				RuntimeError{unknownParamsMessage(params), std::move(currentStacktrace)} {}
		};

		class UnknownSkillTypeError : public util::RuntimeError {
		public:
			UnknownSkillTypeError(std::string_view type, util::Stacktrace currentStacktrace = {}) noexcept :
				RuntimeError{std::format("Unknown spell type \"{}\"", type),
				std::move(currentStacktrace)} {}
		};

		class UnknownDamageTypeError : public util::RuntimeError {
		public:
			UnknownDamageTypeError(std::string_view damageType, util::Stacktrace currentStacktrace = {}) noexcept :
				RuntimeError{std::format("Unknown damage type \"{}\"", damageType),
				std::move(currentStacktrace)} {}
		};

		DamageType getDamageType(const std::string& name) {
			if (auto v = util::getOptional(damageTypeByName, name))
				return *v;
			else
				throw UnknownDamageTypeError{name};
		}

		std::unique_ptr<ProjectileSpell> createProjectileSpell(std::unordered_map<std::string, std::string>& params, 
			    std::shared_ptr<render::AssetManager> assets, 
			    std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles) {
			double damage = util::parseReal(util::getAndEraseRequired(params, "damage"));
			DamageType damageType = getDamageType(util::getAndEraseRequired(params, "damageType"));

			double accuracy = util::parseReal(util::getAndEraseRequired(params, "accuracy"));

			double manaUsage = util::parseReal(util::getAndEraseRequired(params, "mana"));

			const sf::Texture& icon = assets->texture(util::getAndEraseRequired(params, "icon"));
			std::string name = util::getAndEraseRequired(params, "name");

			sf::Time flightTime = sf::seconds(util::parseReal(util::getAndEraseRequired(params, "flightTime")));
			const sf::Texture& projectileTexture = assets->texture(util::getAndEraseRequired(params, "projectileTexture"));

			if (!params.empty())
				throw UnknownParamsError{params};

			return std::make_unique<ProjectileSpell>(
				ProjectileSpell::Stats{damage, damageType, accuracy, manaUsage, flightTime, &projectileTexture},
				icon, name, world, particles
			);
		}

		std::unique_ptr<FallingProjectileSpell> createFallingProjectileSpell(std::unordered_map<std::string, std::string>& params,
				std::shared_ptr<render::AssetManager> assets,
				std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles) {
			double damage = util::parseReal(util::getAndEraseRequired(params, "damage"));
			DamageType damageType = getDamageType(util::getAndEraseRequired(params, "damageType"));

			double accuracy = util::parseReal(util::getAndEraseRequired(params, "accuracy"));

			double manaUsage = util::parseReal(util::getAndEraseRequired(params, "mana"));

			const sf::Texture& icon = assets->texture(util::getAndEraseRequired(params, "icon"));
			std::string name = util::getAndEraseRequired(params, "name");

			float fallHeight = util::parseReal(util::getAndEraseRequired(params, "fallHeight"));
			sf::Time fallTime = sf::seconds(util::parseReal(util::getAndEraseRequired(params, "fallTime")));
			const sf::Texture& projectileTexture = assets->texture(util::getAndEraseRequired(params, "projectileTexture"));

			if (!params.empty())
				throw UnknownParamsError{params};

			return std::make_unique<FallingProjectileSpell>(
				FallingProjectileSpell::Stats{damage, damageType, accuracy, manaUsage, fallHeight, fallTime, &projectileTexture},
				icon, name, world, particles
			);
		}

		std::unique_ptr<BranchingRaySpell> createBranchingRaySpell(std::unordered_map<std::string, std::string>& params,
			    std::shared_ptr<render::AssetManager> assets,
				std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles, 
				std::shared_ptr<util::Raycaster> raycaster, util::RandomEngine& randomEngine) {
			double damage = util::parseReal(util::getAndEraseRequired(params, "damage"));

			DamageType damageType = getDamageType(util::getAndEraseRequired(params, "damageType"));

			double accuracy = util::parseReal(util::getAndEraseRequired(params, "accuracy"));

			double manaUsage = util::parseReal(util::getAndEraseRequired(params, "mana"));

			const sf::Texture& icon = assets->texture(util::getAndEraseRequired(params, "icon"));
			std::string name = util::getAndEraseRequired(params, "name");

			double chainChance = util::parseReal(util::getAndEraseRequired(params, "chainChance"));

			sf::Time visibleTime = sf::seconds(util::parseReal(util::getAndEraseRequired(params, "visibleTime")));
			const sf::Texture& rayTexture = assets->texture(util::getAndEraseRequired(params, "rayTexture"));

			if (!params.empty())
				throw UnknownParamsError{params};

			return std::make_unique<BranchingRaySpell>(
				BranchingRaySpell::Stats{damage, damageType, accuracy, manaUsage, chainChance, visibleTime, &rayTexture},
				icon, name, world, particles, raycaster, randomEngine
			);
		}


		std::unique_ptr<ChargingRaySpell> createChargingRaySpell(std::unordered_map<std::string, std::string>& params,
				std::shared_ptr<render::AssetManager> assets,
				std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles,
				std::shared_ptr<util::Raycaster> raycaster) {
			double damage = util::parseReal(util::getAndEraseRequired(params, "damage"));
			DamageType damageType = getDamageType(util::getAndEraseRequired(params, "damageType"));
			double damageGrowthMul = util::parseReal(util::getAndEraseRequired(params, "damageGrowthMul"));

			double accuracy = util::parseReal(util::getAndEraseRequired(params, "accuracy"));

			double manaUsage = util::parseReal(util::getAndEraseRequired(params, "mana"));

			const sf::Texture& icon = assets->texture(util::getAndEraseRequired(params, "icon"));
			std::string name = util::getAndEraseRequired(params, "name");
			
			sf::Time minVisibleTime = sf::seconds(util::parseReal(util::getAndEraseRequired(params, "minVisibleTime")));
			const sf::Texture& rayTexture = assets->texture(util::getAndEraseRequired(params, "rayTexture"));

			if (!params.empty())
				throw UnknownParamsError{params};

			return std::make_unique<ChargingRaySpell>(
				ChargingRaySpell::Stats{damage, damageType, damageGrowthMul, accuracy, manaUsage, minVisibleTime, &rayTexture},
				icon, name, world, particles, raycaster
			);
		}
	}

	SpellManager::SpellManager(std::shared_ptr<render::AssetManager> assets,
							   std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles,
							   std::shared_ptr<util::Raycaster> raycaster, util::RandomEngine& randomEngine,
		                       util::LoggerFactory& loggerFactory) {
		auto logger = loggerFactory.create("effects");
		logger->info("Loading...");
		util::forEachFile("resources/Spells/", [&, this](std::ifstream& file, const std::filesystem::path& path) {
			logger->info("Loading Skill spec from {} ...", path.generic_string());
			auto params = util::parseMapping(file);

			std::string type = util::getAndEraseRequired(params, "type");
			if (type == "projectile") {
				spells.push_back(createProjectileSpell(params, assets, world, particles));
			} else if (type == "fallingProjectile") {
				spells.push_back(createFallingProjectileSpell(params, assets, world, particles));
			} else if (type == "branchingRay") {
				spells.push_back(createBranchingRaySpell(params, assets, world, particles, raycaster, randomEngine));
			} else if (type == "chargingRay") {
				spells.push_back(createChargingRaySpell(params, assets, world, particles, raycaster));
			}
		});

		logger->info("Loaded");
	}
}
