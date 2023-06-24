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

#include "Dungeon.hpp"

#include "geometry.hpp"

#include <algorithm>

void Dungeon::assign(sf::Vector3i newShape, Tile tile) {
	tiles.clear();
	shape_ = newShape;
	tiles.resize(shape_.x * shape_.y * shape_.z, tile);
}

void Dungeon::generate(std::shared_ptr<spdlog::logger> logger) {
	assign({ 50, 50, 10 });

	logger->info("Generating dungeon...");
	areas_.resize(std::ssize(tiles));
	generator().dungeon(*this);
	generator()();

	logger->info("Generating stairs...");
	for (int i = 1; i < shape().z; ++ i)
		generateUpStairs(i);
}

void Dungeon::addStairs(sf::Vector3i pos1, sf::Vector3i pos2) {
	if (pos1.z > pos2.z) {
		addStairs(pos2, pos1);
		return;
	}

	downStairs_.insert_or_assign(pos1, pos2);
	at(pos1) = Tile::DOWN_STAIRS;

	upStairs_.insert_or_assign(pos2, pos1);
	at(pos2) = Tile::UP_STAIRS;
}

void Dungeon::generateUpStairs(int fromLevel) {
	for (int i = 0; i < 3; ++i)
		addStairs(randomPositionAt(fromLevel - 1, &isEmpty), randomPositionAt(fromLevel, &isEmpty));
}
