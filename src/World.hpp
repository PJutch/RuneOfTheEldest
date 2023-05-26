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
#include "random.hpp"

#include <vector>
#include <memory>

class World {
public:
	World(DungeonGenerator generator_) : 
		generator{ std::move(generator_) } {}

	DungeonGenerator& dungeonGenerator() noexcept {
		return generator;
	}

	const DungeonGenerator& dungeonGenerator() const noexcept {
		return generator;
	}

	// unsafe
	Level& operator[] (int level) {
		return levels[level];
	}

	// unsafe
	const Level& operator[] (int level) const {
		return levels[level];
	}

	// unsafe
	Tile& at(int x, int y, int level) noexcept {
		return (*this)[level].at(x, y);
	}

	// unsafe
	const Tile& at(int x, int y, int level) const noexcept {
		return (*this)[level].at(x, y);
	}

	// unsafe
	Tile& at(sf::Vector2i position, int level) noexcept {
		return at(position.x, position.y, level);
	}

	// unsafe
	const Tile& at(sf::Vector2i position, int level) const noexcept {
		return at(position.x, position.y, level);
	}

	// unsafe
	Tile& at(sf::Vector3i position) noexcept {
		return at(position.x, position.y, position.z);
	}

	// unsafe
	const Tile& at(sf::Vector3i position) const noexcept {
		return at(position.x, position.y, position.z);
	}

	int size() const noexcept {
		return levels.size();
	}

	void generate(std::shared_ptr<spdlog::logger> logger, RandomEngine& randomEgine);
private:
	std::vector<Level> levels;
	DungeonGenerator generator;

	void addStairs(sf::Vector3i pos1, sf::Vector3i pos2);
	void generateUpStairs(int fromLevel, std::shared_ptr<spdlog::logger> logger, RandomEngine& randomEgine);
};

#endif