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

#ifndef SOUND_HPP_
#define SOUND_HPP_

#include <SFML/System/Vector3.hpp>

#include <string_view>
#include <string>

namespace core {
	struct Sound {
		/// Action caused sound
		enum class Type {
			WALK,   ///< Actor moved
			ATTACK, ///< Actor attacked
			TOTAL   ///< Technical enumerator. Should be last
		};

		static const int totalTypes = static_cast<int>(Sound::Type::TOTAL);

		Type type;

		/// Sound source side
		bool isSourceOnPlayerSide;

		/// Sound source position
		sf::Vector3i position;

		friend bool operator == (const Sound&, const Sound&) = default;

		/// @brief Volume of Sound heard from listenerPosition
		/// @details Sound volume depends on the type.
		/// It reduces quadratically with the distance.
		/// Sounds can't be heard from other Levels
		double volume(sf::Vector3i listenerPosition);

		[[nodiscard]] static Sound parse(std::string_view data);
		[[nodiscard]] std::string stringify() const;
	};
}

#endif