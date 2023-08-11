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

#include "XpManager.hpp"

#include "Actor.hpp"

#include "util/parse.hpp"
#include "util/Map.hpp"
#include "util/filesystem.hpp"

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
}

namespace core {
	XpManager::XpManager(std::shared_ptr<World> world_, std::shared_ptr<render::AssetManager> assets, util::RandomEngine& randomEngine_) :
			world{std::move(world_)}, randomEngine{&randomEngine_} {
		util::forEachFile("resources/Skills/", [this, &assets](std::ifstream& file) {
			auto params = util::parseMapping(file);

			double regenMul = util::parseReal(util::getAndEraseRequired(params, "regenMul"));
			const sf::Texture& icon = assets->texture(util::getAndEraseRequired(params, "icon"));
			std::string name = util::getAndEraseRequired(params, "name");

			if (!params.empty())
				throw UnknownParamsError{params};

			skills.push_back(std::make_unique<RegenSkill>(regenMul, icon, name));
		});
	}

	void XpManager::generateAvailableSkills() {
		availableSkills_.resize(3);
		for (const Skill*& skill : availableSkills_) {
			auto iskill = std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(skills) - 1}(*randomEngine);
			skill = skills[iskill].get();
		}
	}

	void XpManager::levelUp(const Skill* skill) {
		world->player().addSkill(skill->clone());
		xp -= xpUntilNextLvl;
		xpUntilNextLvl *= 2;
	}
}
