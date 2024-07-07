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

#include "EffectManager.hpp"

#include "Effect/Skill.hpp"
#include "Effect/LowHpSkill.hpp"
#include "Effect/TargetFullHpSkill.hpp"
#include "Effect/Poison.hpp"
#include "Effect/AppliesEffectOnAttack.hpp"
#include "Effect/TempBonus.hpp"
#include "Effect/ConditionalBonus.hpp"

#include "core/DamageType.hpp"

#include "render/AssetManager.hpp"

#include "util/parse.hpp"
#include "util/parseKeyValue.hpp"
#include "util/Map.hpp"
#include "util/filesystem.hpp"
#include "util/case.hpp"

#include <JutchsON.hpp>

namespace core {
	namespace {
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

		struct Env {
			std::shared_ptr<render::AssetManager> assets;
			std::string_view id;
		};
	}
}

namespace JutchsON {
	template <std::derived_from<core::Effect> Effect>
	struct Parser<Effect> {
		ParseResult<Effect> operator() (StringView s, const auto& env, Context context) {
			return parse<typename Effect::Data>(s, env, context).map([&](auto data) {
				return Effect{data, env.id};
			});
		}
	};
}

namespace core {
	EffectManager::EffectManager(std::shared_ptr<render::AssetManager> assets,
		                         util::LoggerFactory& loggerFactory) {
		auto logger = loggerFactory.create("effects");
		logger->info("Loading...");

		std::filesystem::path basePath = "resources/descriptions/Effects/";
		util::forEachFile(basePath, [&, this](std::ifstream& file, const std::filesystem::path& path) {
			std::string id = util::toIdentifier(path, basePath);

			logger->info("Loading {} skill spec from {}...", id, path.generic_string());

			std::tuple typenames{
				JUTCHSON_TAGGED_TYPE_NAME(TempBonus),
				JUTCHSON_TAGGED_TYPE_NAME(Poison),
				JUTCHSON_TAGGED_TYPE_NAME(AppliesEffectOnAttack),
				JUTCHSON_TAGGED_TYPE_NAME(TargetFullHpSkill),
				JUTCHSON_TAGGED_TYPE_NAME(Skill),
				JUTCHSON_TAGGED_TYPE_NAME(LowHpSkill)
			};

			auto fileStr = JutchsON::readWholeFile(path);
			if (auto result = JutchsON::parseTypeVariant<Effect>(fileStr, typenames, Env{assets, id})) {
				effects.push_back(std::move(*result));
			} else {
				throw ParseError{result.errors()};
			}
		});

		logger->info("Initializing...");
		for (const auto& effect : effects)
			effect->init(*this);

		logger->info("Loaded");
	}
}
