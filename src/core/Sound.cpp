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

#include <array>

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
}
