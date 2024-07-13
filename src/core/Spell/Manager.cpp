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

namespace JutchsON {
	template <>
	struct Parser<sf::Time> {
		ParseResult<sf::Time> operator() (StringView s, const auto& env, Context context) {
			return parse<float>(s, env, context).map([](float t) {
				return sf::seconds(t);
			});
		}
	};

	template <>
	struct Parser<core::DamageType> {
		ParseResult<core::DamageType> operator() (StringView s, const auto& env, Context context) {
			return parse<std::string>(s, env, context).then([&](std::string_view name) {
				auto result = ParseResult<core::DamageType>::makeError(s.location(), 
					std::format("Unknown damage type {}", s.asStd()));
				boost::mp11::mp_for_each<boost::describe::describe_enumerators<core::DamageType>>([&](const auto& d) {
					if (d.name == name) {
						result = d.value;
					}
				});
				return result;
			});
		}
	};

	template <>
	struct Parser<const core::Effect*> {
		ParseResult<const core::Effect*> operator() (StringView s, const auto& env, Context context) {
			return parse<std::string>(s, env, context).then([&](std::string_view name) -> ParseResult<const core::Effect*> {
				if (auto effect = env.effects->findEffect(name)) {
					return effect;
				}
				return ParseResult<const core::Effect*>::makeError(s.location(), 
					std::format("Unknown effect {}", s.asStd()));
			});
		}
	};

	template <std::derived_from<core::Spell> Spell>
	struct Parser<Spell> {
		ParseResult<Spell> operator() (StringView s, const auto& env, Context context) {
			return parse<typename Spell::Stats>(s, env, context).map([&](auto data) {
				return Spell{data, env};
			});
		}
	};

	template <>
	struct Parser<std::unique_ptr<core::Spell>> {
		template <typename Env>
		ParseResult<std::unique_ptr<core::Spell>> operator() (StringView s, Env&& env, Context context) {
			using namespace core;

			std::tuple typenames{
				JUTCHSON_TAGGED_TYPE_NAME(BanishSpell),
				JUTCHSON_TAGGED_TYPE_NAME(BonusSpell),
				JUTCHSON_TAGGED_TYPE_NAME(BranchingRaySpell),
				JUTCHSON_TAGGED_TYPE_NAME(ChargingRaySpell),
				JUTCHSON_TAGGED_TYPE_NAME(ConcentrationBonusSpell),
				JUTCHSON_TAGGED_TYPE_NAME(DigSpell),
				JUTCHSON_TAGGED_TYPE_NAME(HealSpell),
				JUTCHSON_TAGGED_TYPE_NAME(FallingProjectileSpell),
				JUTCHSON_TAGGED_TYPE_NAME(ExplodingProjectileSpell),
				JUTCHSON_TAGGED_TYPE_NAME(IdentifySpell),
				JUTCHSON_TAGGED_TYPE_NAME(MagicMappingSpell),
				JUTCHSON_TAGGED_TYPE_NAME(ProjectileSpell),
				JUTCHSON_TAGGED_TYPE_NAME(RadianceSpell),
				JUTCHSON_TAGGED_TYPE_NAME(RingSpell),
				JUTCHSON_TAGGED_TYPE_NAME(SenseItemsSpell),
				JUTCHSON_TAGGED_TYPE_NAME(SenseMonstersSpell),
				JUTCHSON_TAGGED_TYPE_NAME(TeleportSpell)
			};

			return JutchsON::parseTypeVariant<core::Spell>(s, typenames, std::forward<Env>(env));
		}
	};
}

namespace core {
	namespace {
		struct Env {
			std::string_view id;
			std::shared_ptr<World> world;
			std::shared_ptr<EffectManager> effects;
			std::shared_ptr<render::AssetManager> assets;
			std::shared_ptr<render::ParticleManager> particles;
			std::shared_ptr<render::PlayerMap> playerMap;
			std::shared_ptr<util::Raycaster> raycaster;
			util::RandomEngine* randomEngine;
		};

		std::string errorString(const std::vector<JutchsON::ParseError>& errors) {
			std::string res;
			res.append(std::format("Parse failed with {} errors", std::ssize(errors)));
			for (const JutchsON::ParseError& error : errors) {
				res.append(std::format("\n{}:{} {}", error.location.line, error.location.column, error.what));
			}
			return res;
		}

		class ParseError : public util::RuntimeError {
		public:
			ParseError(const std::vector<JutchsON::ParseError>& errors, util::Stacktrace currentStacktrace = {}) noexcept :
				RuntimeError{errorString(errors), std::move(currentStacktrace)} {}
		};
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

			if (auto spell = JutchsON::parseFile<std::unique_ptr<Spell>>(path, 
					Env{id, world, effectManager, assets, particles, playerMap, raycaster, &randomEngine})) {
				spells.emplace_back(std::move(*spell));
			} else {
				throw ParseError{spell.errors()};
			}
		});

		logger->info("Loaded");
	}
}
