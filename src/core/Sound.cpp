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

#include "Sound.hpp"

#include "util/parse.hpp"
#include "util/parseKeyValue.hpp"
#include "util/stringify.hpp"
#include "util/Exception.hpp"

#include <array>
#include <cstddef>

namespace core {
	namespace {
		const std::array<double, Sound::totalTypes> basicVolumes{ 0.1, 0.5 };
	}

	double Sound::volume(sf::Vector3i listenerPosition) {
		if (position.z != listenerPosition.z)
			return 0.0;

		auto [dx, dy, dz] = position - listenerPosition;
		int distance = dx * dx + dy * dy;

		return basicVolumes[static_cast<ptrdiff_t>(type)] / distance;
	}

	namespace {
		class UnknowType : public util::RuntimeError {
		public:
			UnknowType(std::string_view type, util::Stacktrace stacktrace = {}) : 
				util::RuntimeError{std::format("Unknown sound type {}", type), std::move(stacktrace)} {}
		};
	}

	[[nodiscard]] Sound Sound::parse(std::string_view data) {
		Sound result;

		util::KeyValueVisitor visitor;
		visitor.key("type").unique().required().callback([&](std::string_view type) {
			if (type == "walk") {
				result.type = Sound::Type::WALK;
			} else if (type == "attack") {
				result.type = Sound::Type::WALK;
			} else {
				throw UnknowType{type};
			}
		});
		visitor.key("position").unique().required().callback([&](std::string_view type) {
			result.position = util::parseVector3i(type);
		});
		visitor.key("isSourceOnPlayerSide").unique().required().callback([&](std::string_view type) {
			result.isSourceOnPlayerSide = util::parseBool(type);
		});

		util::forEackKeyValuePair(data, visitor);
		visitor.validate();

		return result;
	}

	namespace {
		[[nodiscard]] std::string typeName(Sound::Type type) {
			switch (type) {
			case Sound::Type::WALK: return "walk";
			case Sound::Type::ATTACK: return "attack";
			default: TROTE_ASSERT(false, "unreachable");
			}
		}
	}

	[[nodiscard]] std::string Sound::stringify() const {
		return std::format("type {}\nposition {}\nisSourceOnPlayerSide {}\n", 
			               typeName(type), util::stringifyVector3(position), isSourceOnPlayerSide);
	}
}
