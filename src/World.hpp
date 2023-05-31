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
	World(std::unique_ptr<Dungeon> dungeon_) : dungeon{ std::move(dungeon_) } {}

	DungeonGenerator& dungeonGenerator() noexcept {
		return dungeon->dungeonGenerator();
	}

	const DungeonGenerator& dungeonGenerator() const noexcept {
		return dungeon->dungeonGenerator();
	}

	/// @brief Access to individual level
	/// @warning Check level index by yourself
	///       \n You may use size
	Level& operator[] (int level) {
		return (*dungeon)[level];
	}

	/// @brief Access to individual level
	/// @warning Check level index by yourself
	///       \n You may use size
	const Level& operator[] (int level) const {
		return (*dungeon)[level];
	}

	/// @brief Access to individual tile on level level at (x, y)
	/// @warning Check all indices by yourself
	Tile& at(int x, int y, int level) noexcept {
		return dungeon->at(x, y, level);
	}

	/// @brief Access to individual tile on level level at (x, y)
	/// @warning Check all indices by yourself
	const Tile& at(int x, int y, int level) const noexcept {
		return dungeon->at(x, y, level);
	}

	/// @brief Access to individual tile on level level at (x, y)
	/// @warning Check all indices by yourself
	Tile& at(sf::Vector2i position, int level) noexcept {
		return dungeon->at(position, level);
	}

	/// @brief Access to individual tile on level level at (x, y)
	/// @warning Check all indices by yourself
	const Tile& at(sf::Vector2i position, int level) const noexcept {
		return dungeon->at(position, level);
	}

	/// @brief Access to individual tile on level z at (x, y)
	/// @warning Check all indices by yourself
	Tile& at(sf::Vector3i position) noexcept {
		return dungeon->at(position);
	}

	/// @brief Access to individual tile on level z at (x, y)
	/// @warning Check all indices by yourself
	const Tile& at(sf::Vector3i position) const noexcept {
		return dungeon->at(position);
	}

	/// Level count
	int size() const noexcept {
		return dungeon->size();
	}

	/// Returns at(position) destination if it's Tile::UP_STAIRS
	std::optional<sf::Vector3i> upStairs(sf::Vector3i position) {
		return dungeon->upStairs(position);
	}

	/// Returns at(position) destination if it's Tile::DOWN_STAIRS
	std::optional<sf::Vector3i> downStairs(sf::Vector3i position) {
		return dungeon->downStairs(position);
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
	std::unique_ptr<Dungeon> dungeon;
};

#endif