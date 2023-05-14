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

#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "DungeonGenerator.hpp"
#include "Level.hpp"

#include "log.hpp"

#include <vector>

class World {
public:
	World() = default;

	Level& operator[] (int index) {
		return levels[index];
	}

	const Level& operator[] (int index) const {
		return levels[index];
	}

	void generate(DungeonGenerator& generator, spdlog::logger& logger);
private:
	std::vector<Level> levels;
};

#endif