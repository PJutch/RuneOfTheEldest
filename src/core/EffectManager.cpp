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

#include "Effect/UnconditionalSkill.hpp"
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

		template <typename Skill>
		std::unique_ptr<Skill> parseSkillByJutchsON(JutchsON::StringView s, std::string_view id,
											        std::shared_ptr<render::AssetManager> assets) {
			if (auto data = JutchsON::parse<typename Skill::Data>(s, {}, JutchsON::Context::LINE_REST)) {
				return std::make_unique<Skill>(*data, id, assets);
			} else {
				throw ParseError{data.errors()};
			}
		}
	}

	EffectManager::EffectManager(std::shared_ptr<render::AssetManager> assets,
		                         util::LoggerFactory& loggerFactory) {
		auto logger = loggerFactory.create("effects");
		logger->info("Loading...");

		std::filesystem::path basePath = "resources/descriptions/Effects/";
		util::forEachFile(basePath, [&, this](std::ifstream& file, const std::filesystem::path& path) {
			std::string id = util::toIdentifier(path, basePath);

			logger->info("Loading {} skill spec from {}...", id, path.generic_string());

			auto fileStr = JutchsON::readWholeFile(path);
			auto parsed = JutchsON::parseVariant(fileStr);
			if (!parsed) {
				throw ParseError{parsed.errors()};
			}

			if (parsed->first == "tempBonus") {
				effects.push_back(parseSkillByJutchsON<TempBonus>(parsed->second, id, assets));
			} else if(parsed->first == "poison") {
				effects.push_back(parseSkillByJutchsON<Poison>(parsed->second, id, assets));
			} else if (parsed->first == "appliesEffectOnAttack") {
				effects.push_back(parseSkillByJutchsON<AppliesEffectOnAttack>(parsed->second, id, assets));
			} else if (parsed->first == "targetFullHpSkill") {
				effects.push_back(parseSkillByJutchsON<TargetFullHpSkill>(parsed->second, id, assets));
			} else if (parsed->first == "skill") {
				effects.push_back(parseSkillByJutchsON<UnconditionalSkill>(parsed->second, id, assets));
			} else if (parsed->first == "lowHpSkill") {
				effects.push_back(parseSkillByJutchsON<LowHpSkill>(parsed->second, id, assets));
			} else {
				throw UnknownSkillTypeError(parsed->first.asStd());
			}
		});

		logger->info("Initializing...");
		for (const auto& effect : effects)
			effect->init(*this);

		logger->info("Loaded");
	}
}
