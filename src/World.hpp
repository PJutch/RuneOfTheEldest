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

#include "Dungeon.hpp"

/// Dungeons and all objects in it
class World {
public:
	World(std::shared_ptr<Dungeon> dungeon_) : dungeon{ std::move(dungeon_) } {}

	DungeonGenerator& dungeonGenerator() noexcept {
		return dungeon->dungeonGenerator();
	}

	const DungeonGenerator& dungeonGenerator() const noexcept {
		return dungeon->dungeonGenerator();
	}

	/// @brief Generates dungeon
	/// @details for each level 
	/// calls generateBlank, applies DungeonGenerator, 
	/// calls generateWalls and generateStairs
	/// 
	/// @param logger logger to log messages
	void generate(std::shared_ptr<spdlog::logger> logger) {
		dungeon->generate(std::move(logger));
	}
private:
	std::shared_ptr<Dungeon> dungeon;
};

#endif