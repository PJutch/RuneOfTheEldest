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
				RuntimeError{std::format("Unknown skill type \"{}\"", type),
				std::move(currentStacktrace)} {}
		};

		std::unique_ptr<ProjectileSpell> createProjectileSpell(std::unordered_map<std::string, std::string>& params, 
			    std::shared_ptr<render::AssetManager> assets, 
			    std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles) {
			double damage = util::parseReal(util::getAndEraseRequired(params, "damage"));
			DamageType damageType = damageTypeByName.at(util::getAndEraseRequired(params, "damageType"));

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
	}

	SpellManager::SpellManager(std::shared_ptr<render::AssetManager> assets,
							   std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles,
		                       util::LoggerFactory& loggerFactory) {
		auto logger = loggerFactory.create("effects");
		logger->info("Loading...");
		util::forEachFile("resources/Spells/", [&, this](std::ifstream& file, const std::filesystem::path& path) {
			logger->info("Loading Skill spec from {} ...", path.generic_string());
			auto params = util::parseMapping(file);
			spells.push_back(createProjectileSpell(params, assets, world, particles));
		});

		logger->info("Loaded");
	}
}
