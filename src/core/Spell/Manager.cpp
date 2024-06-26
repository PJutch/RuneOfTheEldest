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
#include "ExplodingProjectile.hpp"
#include "Ring.hpp"
#include "Teleport.hpp"
#include "Dig.hpp"
#include "Heal.hpp"
#include "Bonus.hpp"
#include "ConcentrationBonus.hpp"
#include "Radiance.hpp"
#include "MagicMapping.hpp"
#include "SenseMonsters.hpp"
#include "SenseItems.hpp"
#include "Banish.hpp"
#include "Identify.hpp"

#include "core/DamageType.hpp"

#include "render/AssetManager.hpp"

#include "util/parse.hpp"
#include "util/parseKeyValue.hpp"
#include "util/Map.hpp"
#include "util/filesystem.hpp"

#include <boost/mp11.hpp>

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

		template <typename T>
		struct parseImpl {};

		template <std::floating_point T>
		struct parseImpl<T> {
			static T parse(std::string_view s, EffectManager&, render::AssetManager&) {
				return util::parseReal<T>(s);
			}
		};

		template <>
		struct parseImpl<DamageType> {
			static DamageType parse(std::string_view sv, EffectManager&, render::AssetManager&) {
				std::string str{sv};
				return getDamageType(str);
			}
		};

		template <>
		struct parseImpl<const sf::Texture*> {
			static const sf::Texture* parse(std::string_view s, EffectManager&, render::AssetManager& assets) {
				return &assets.texture(s);
			}
		};

		template <>
		struct parseImpl<sf::Time> {
			static sf::Time parse(std::string_view s, EffectManager&, render::AssetManager&) {
				return sf::seconds(util::parseReal(s));
			}
		};

		template <>
		struct parseImpl<const Effect*> {
			static const Effect* parse(std::string_view s, EffectManager& effects, render::AssetManager&) {
				return effects.findEffect(s);
			}
		};

		class ParseNotImplemented : public util::RuntimeError {
		public:
			ParseNotImplemented(std::string_view type, util::Stacktrace currentStacktrace = {}) noexcept :
				RuntimeError{std::format("Parse not implemented for {}", type),
							 std::move(currentStacktrace)} {}
		};

		template <>
		struct parseImpl<ActorImpact> {
			static ActorImpact parse(std::string_view, EffectManager&, render::AssetManager&) {
				throw ParseNotImplemented{"ActorImpact"};
			}
		};

		template <typename Result>
			requires std::constructible_from<Result, typename Result::Stats, const sf::Texture&, std::string_view, std::string_view>
		std::unique_ptr<Result> makeSpell(typename Result::Stats stats,
				const sf::Texture& icon, std::string_view id, std::string_view name,
				std::shared_ptr<World>, std::shared_ptr<render::ParticleManager>,
				std::shared_ptr<render::PlayerMap>,
				std::shared_ptr<util::Raycaster>, util::RandomEngine&) {
			return std::make_unique<Result>(stats, icon, id, name);
		}

		template <typename Result>
			requires std::constructible_from<Result, 
				typename Result::Stats, const sf::Texture&, std::string_view, std::string_view, std::shared_ptr<World>>
		std::unique_ptr<Result> makeSpell(typename Result::Stats stats,
				const sf::Texture& icon, std::string_view id, std::string_view name,
				std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager>,
				std::shared_ptr<render::PlayerMap>,
				std::shared_ptr<util::Raycaster>, util::RandomEngine&) {
			return std::make_unique<Result>(stats, icon, id, name, world);
		}

		template <typename Result>
			requires std::constructible_from<Result,
		typename Result::Stats, const sf::Texture&, std::string_view, std::string_view, std::shared_ptr<render::PlayerMap>>
			std::unique_ptr<Result> makeSpell(typename Result::Stats stats,
				const sf::Texture& icon, std::string_view id, std::string_view name,
				std::shared_ptr<World>, std::shared_ptr<render::ParticleManager>,
				std::shared_ptr<render::PlayerMap> playerMap,
				std::shared_ptr<util::Raycaster>, util::RandomEngine&) {
			return std::make_unique<Result>(stats, icon, id, name, playerMap);
		}

		template <typename Result>
			requires std::constructible_from<Result, typename Result::Stats,
				const sf::Texture&, std::string_view, std::string_view,
				std::shared_ptr<World>, std::shared_ptr<render::ParticleManager>>
		std::unique_ptr<Result> makeSpell(typename Result::Stats stats,
				const sf::Texture& icon, std::string_view id, std::string_view name,
				std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles,
				std::shared_ptr<render::PlayerMap>,
				std::shared_ptr<util::Raycaster>, util::RandomEngine&) {
			return std::make_unique<Result>(stats, icon, id, name, world, particles);
		}

		template <typename Result>
			requires std::constructible_from<Result, typename Result::Stats,
				const sf::Texture&, std::string_view, std::string_view,
				std::shared_ptr<World>, std::shared_ptr<render::ParticleManager>,
				std::shared_ptr<util::Raycaster>>
		std::unique_ptr<Result> makeSpell(typename Result::Stats stats,
				const sf::Texture& icon, std::string_view id, std::string_view name,
				std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles,
				std::shared_ptr<render::PlayerMap>,
				std::shared_ptr<util::Raycaster> raycaster, util::RandomEngine&) {
			return std::make_unique<Result>(stats, icon, id, name, world, particles, raycaster);
		}

		template <typename Result> 
			requires std::constructible_from<Result, typename Result::Stats,
				const sf::Texture&, std::string_view, std::string_view,
				std::shared_ptr<World>, std::shared_ptr<render::ParticleManager>,
				std::shared_ptr<util::Raycaster>, util::RandomEngine&>
		std::unique_ptr<Result> makeSpell(typename Result::Stats stats,
				const sf::Texture& icon, std::string_view id, std::string_view name,
				std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles,
				std::shared_ptr<render::PlayerMap>,
				std::shared_ptr<util::Raycaster> raycaster, util::RandomEngine& randomEngine) {
			return std::make_unique<Result>(stats, icon, id, name, world, particles, raycaster, randomEngine);
		}

		template <typename Result>
			requires std::constructible_from<Result, typename Result::Stats,
				const sf::Texture&, std::string_view, std::string_view,
				std::shared_ptr<World>, std::shared_ptr<render::ParticleManager>,
				std::shared_ptr<render::PlayerMap>>
		std::unique_ptr<Result> makeSpell(typename Result::Stats stats,
				const sf::Texture& icon, std::string_view id, std::string_view name,
				std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles,
				std::shared_ptr<render::PlayerMap> playerMap,
				std::shared_ptr<util::Raycaster>, util::RandomEngine&) {
			return std::make_unique<Result>(stats, icon, id, name, world, particles, playerMap);
		}

		template <typename Result>
			requires std::constructible_from<Result, typename Result::Stats,
				const sf::Texture&, std::string_view, std::string_view,
				std::shared_ptr<World>, std::shared_ptr<render::ParticleManager>,
				std::shared_ptr<render::PlayerMap>, std::shared_ptr<util::Raycaster>>
		std::unique_ptr<Result> makeSpell(typename Result::Stats stats,
				const sf::Texture& icon, std::string_view id, std::string_view name,
				std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles,
				std::shared_ptr<render::PlayerMap> playerMap,
				std::shared_ptr<util::Raycaster> raycaster, util::RandomEngine&) {
			return std::make_unique<Result>(stats, icon, id, name, world, particles, playerMap, raycaster);
		}

		class UnknownEffect : public util::RuntimeError {
		public:
			UnknownEffect(std::string_view effect, util::Stacktrace currentStacktrace = {}) noexcept :
				RuntimeError{std::format("Effect \"{}\" not found", effect), std::move(currentStacktrace)} {}
		};

		ActorImpact loadImpact(std::unordered_map<std::string, std::string>& params, EffectManager& effects) {
			ActorImpact impact;
			impact.accuracy(util::parseReal(util::getAndEraseRequired(params, "accuracy")));

			if (auto damageStr = util::getAndErase(params, "damage")) {
				auto damageType = getDamageType(util::getAndEraseRequired(params, "damageType"));
				impact.damage(util::parseReal(*damageStr), damageType);
			}
			
			if (auto v = util::getAndErase(params, "effect")) {
				if (auto effect = effects.findEffect(*v))
					impact.effect(effect);
				else
					throw UnknownEffect{*v};
			}

			return impact;
		}

		template <typename Loaded>
		std::unique_ptr<Loaded> loadSpell(
				std::unordered_map<std::string, std::string>& params, std::string_view id,
				std::shared_ptr<EffectManager> effects, std::shared_ptr<render::AssetManager> assets,
				std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles,
				std::shared_ptr<render::PlayerMap> playerMap,
				std::shared_ptr<util::Raycaster> raycaster, util::RandomEngine& randomEngine) {
			const sf::Texture& icon = assets->texture(util::getAndEraseRequired(params, "icon"));
			std::string name = util::getAndEraseRequired(params, "name");

			using Stats = typename Loaded::Stats;
			Stats stats;

			using members = boost::describe::describe_members<Stats, boost::describe::mod_public>;
			boost::mp11::mp_for_each<members>([&](auto desc) {
				using Member = std::remove_cvref_t<decltype(stats.*desc.pointer)>;

				if (desc.name != "impact") {
					auto valueStr = util::getAndEraseRequired(params, desc.name);
					stats.*desc.pointer = parseImpl<Member>::parse(valueStr, *effects, *assets);
				}
			});

			if constexpr (requires { stats.impact; }) {
				stats.impact = loadImpact(params, *effects);
			}

			if (!params.empty())
				throw UnknownParamsError{params};

			return makeSpell<Loaded>(stats, icon, id, name, world, particles, playerMap, raycaster, randomEngine);
		}

		std::shared_ptr<BonusSpell> loadBonusSpell(
				std::unordered_map<std::string, std::string>& params, std::string_view id,
				std::shared_ptr<render::AssetManager> assets) {
			const sf::Texture& icon = assets->texture(util::getAndEraseRequired(params, "icon"));
			std::string name = util::getAndEraseRequired(params, "name");

			double mana = util::parseReal(util::getAndEraseRequired(params, "mana"));
			return std::make_shared<BonusSpell>(loadBonuses(params), mana, icon, id, name);
		}

		std::shared_ptr<ConcentrationBonusSpell> loadConcentrationBonusSpell(
				std::unordered_map<std::string, std::string>& params, std::string_view id,
				std::shared_ptr<render::AssetManager> assets, std::shared_ptr<render::ParticleManager> particles) {
			const sf::Texture& icon = assets->texture(util::getAndEraseRequired(params, "icon"));
			std::string name = util::getAndEraseRequired(params, "name");

			double mana = util::parseReal(util::getAndEraseRequired(params, "mana"));
			const sf::Texture& particleTexture = assets->texture(util::getAndEraseRequired(params, "particle"));

			return std::make_shared<ConcentrationBonusSpell>(loadBonuses(params), mana, particleTexture, 
															 icon, id, name, std::move(particles));
		}
	}

	SpellManager::SpellManager(std::shared_ptr<core::EffectManager> effectManager, 
							   std::shared_ptr<render::AssetManager> assets,
							   std::shared_ptr<World> world, std::shared_ptr<render::ParticleManager> particles,
							   std::shared_ptr<render::PlayerMap> playerMap,
							   std::shared_ptr<util::Raycaster> raycaster, util::RandomEngine& randomEngine,
		                       util::LoggerFactory& loggerFactory) {
		auto logger = loggerFactory.create("spells");
		logger->info("Loading...");

		std::filesystem::path basePath = "resources/descriptions/Spells/";
		util::forEachFile(basePath, [&, this](std::ifstream& file, const std::filesystem::path& path) {
			std::string id = util::toIdentifier(path, basePath);
			logger->info("Loading {} spell spec from {}...", id, path.generic_string());
			auto params = util::parseMapping(file);

			std::string type = util::getAndEraseRequired(params, "type");
			if (type == "projectile") {
				spells.push_back(loadSpell<ProjectileSpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "fallingProjectile") {
				spells.push_back(loadSpell<FallingProjectileSpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "branchingRay") {
				spells.push_back(loadSpell<BranchingRaySpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "chargingRay") {
				spells.push_back(loadSpell<ChargingRaySpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "explodingProjectile") {
				spells.push_back(loadSpell<ExplodingProjectileSpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "ring") {
				spells.push_back(loadSpell<RingSpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "teleport") {
				spells.push_back(loadSpell<TeleportSpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "dig") {
				spells.push_back(loadSpell<DigSpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "heal") {
				spells.push_back(loadSpell<HealSpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "radiance") {
				spells.push_back(loadSpell<RadianceSpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "magicMapping") {
				spells.push_back(loadSpell<MagicMappingSpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "senseMonsters") {
				spells.push_back(loadSpell<SenseMonstersSpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "senseItems") {
				spells.push_back(loadSpell<SenseItemsSpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "banish") {
				spells.push_back(loadSpell<BanishSpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "identify") {
				spells.push_back(loadSpell<IdentifySpell>(params, id, effectManager, assets, world, particles, playerMap, raycaster, randomEngine));
			} else if (type == "bonus") {
				spells.push_back(loadBonusSpell(params, id, assets));
			} else if (type == "concentrationBonus") {
				spells.push_back(loadConcentrationBonusSpell(params, id, assets, particles));
			}
		});

		logger->info("Loaded");
	}
}
