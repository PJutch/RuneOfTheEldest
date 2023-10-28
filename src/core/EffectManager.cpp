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

#include "core/DamageType.hpp"

#include "render/AssetManager.hpp"

#include "util/parse.hpp"
#include "util/Map.hpp"
#include "util/filesystem.hpp"

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

		std::unique_ptr<Effect> createSkill(std::unordered_map<std::string, std::string>& params,
				std::string_view type, std::string_view name, const sf::Texture& icon) {
			double regenBonus = 1;
			double speedBonus = 0;
			double accuracyBonus = 0;
			double evasionBonus = 0;
			double xpMul = 1;
			double hpBonus = 1;
			std::array<double, totalDamageTypes> defenceBonuses;
			defenceBonuses.fill(0);

			if (type == "unconditionalSkill")
				if (auto v = util::getAndErase(params, "hpBonus"))
					hpBonus = util::parseReal(*v);

			if (type != "targetFullHpSkill") {
				if (auto v = util::getAndErase(params, "regenBonus"))
					regenBonus = util::parseReal(*v);

				if (auto v = util::getAndErase(params, "speed"))
					speedBonus = util::parseReal(*v);

				if (auto v = util::getAndErase(params, "accuracy"))
					accuracyBonus = util::parseReal(*v);

				if (auto v = util::getAndErase(params, "evasion"))
					evasionBonus = util::parseReal(*v);

				if (auto v = util::getAndErase(params, "xpMul"))
					xpMul = util::parseReal(*v);

				for (int i = 0; i < totalDamageTypes; ++i) {
					if (auto v = util::getAndErase(params, damageTypeNames[i] + "DefenceBonus"))
						defenceBonuses[i] = util::parseReal(*v);
				}
			}

			double damageBonus = 1;
			if (auto v = util::getAndErase(params, "damageBonus"))
				damageBonus = util::parseReal(*v);

			if (!params.empty())
				throw UnknownParamsError{params};

			if (type == "unconditionalSkill")
				return std::make_unique<UnconditionalSkill>(
					regenBonus, damageBonus, speedBonus,
					accuracyBonus, evasionBonus, xpMul, hpBonus, defenceBonuses,
					icon, name);
			else if (type == "lowHpSkill")
				return std::make_unique<LowHpSkill>(
					regenBonus, damageBonus, speedBonus, accuracyBonus, evasionBonus, xpMul, defenceBonuses,
					icon, name);
			else if (type == "targetFullHpSkill")
				return std::make_unique<TargetFullHpSkill>(
					damageBonus, icon, name);
			else
				throw UnknownSkillTypeError(type);
		}

		std::unique_ptr<Poison> createPoison(std::unordered_map<std::string, std::string>& params,
			std::string_view name, const sf::Texture& icon) {
			double damageOverTime = 1;
			double duration = 1;

			if (auto v = util::getAndErase(params, "damageOverTime"))
				damageOverTime = util::parseReal(*v);

			if (auto v = util::getAndErase(params, "duration"))
				duration = util::parseReal(*v);

			if (!params.empty())
				throw UnknownParamsError{params};

			return std::make_unique<Poison>(damageOverTime, duration, icon, name);
		}

		std::unique_ptr<AppliesEffectOnAttack> createAppliesEffect(std::unordered_map<std::string, std::string>& params,
			std::string_view name, const sf::Texture& icon) {
			std::string appliedName = util::getAndEraseRequired(params, "applies");

			if (!params.empty())
				throw UnknownParamsError{params};

			return std::make_unique<AppliesEffectOnAttack>(appliedName, icon, name);
		}
	}

	EffectManager::EffectManager(std::shared_ptr<render::AssetManager> assets,
		                         util::LoggerFactory& loggerFactory) {
		auto logger = loggerFactory.create("effects");
		logger->info("Loading...");
		util::forEachFile("resources/Effects/", [&, this](std::ifstream& file, const std::filesystem::path& path) {
			logger->info("Loading Skill spec from {} ...", path.generic_string());

			auto params = util::parseMapping(file);

			std::string type = "unconditionalSkill";
			if (auto v = util::getAndErase(params, "type"))
				type = *v;

			const sf::Texture& icon = assets->texture(util::getAndEraseRequired(params, "icon"));
			std::string name = util::getAndEraseRequired(params, "name");

			if (type == "unconditionalSkill" 
			 || type == "lowHpSkill" 
			 || type == "targetFullHpSkill")
				effects.push_back(createSkill(params, type, name, icon));
			else if (type == "poison")
				effects.push_back(createPoison(params, name, icon));
			else if (type == "appliesEffectOnAttack")
				effects.push_back(createAppliesEffect(params, name, icon));
			else
				throw UnknownSkillTypeError(type);
		});

		logger->info("Initializing...");
		for (const auto& effect : effects)
			effect->init(*this);

		logger->info("Loaded");
	}
}