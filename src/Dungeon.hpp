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

#ifndef DUNGEON_HPP_
#define DUNGEON_HPP_

#include "DungeonGenerator.hpp"
#include "Tile.hpp"

#include "Array3D.hpp"
#include "log.hpp"
#include "random.hpp"
#include "Map.hpp"
#include "geometry.hpp"

#include <vector>
#include <memory>

/// Dungeon consisting of multiple levels and stairs between them
class Dungeon {
public:
	Dungeon() = default;
	Dungeon(std::unique_ptr<DungeonGenerator> newGenerator, RandomEngine& randomEngine_) :
		generator_{ std::move(newGenerator) },
		randomEngine{ &randomEngine_ } {}

	[[nodiscard]] DungeonGenerator& generator() noexcept {
		return *generator_;
	}

	[[nodiscard]] const DungeonGenerator& generator() const noexcept {
		return *generator_;
	}

	/// Checks if there are tiles with this z
	[[nodiscard]] bool isValidZ(int z) const noexcept {
		return tiles.isValidZ(z);
	}

	/// Checks if there are tiles with this x
	[[nodiscard]] bool isValidX(int x) const noexcept {
		return tiles.isValidX(x);
	}

	/// Checks if there are tiles with this y
	[[nodiscard]] bool isValidY(int y) const noexcept {
		return tiles.isValidY(y);
	}

	/// Checks if position is valid tile position
	[[nodiscard]] bool isValidPosition(sf::Vector3i position) const noexcept {
		return tiles.isValidPosition(position);
	}

	/// Checks if rect sides are >=0 and all tiles in it are exists
	[[nodiscard]] bool isValidRect(sf::IntRect rect) const noexcept {
		return isValidX(rect.left) && isValidY(rect.top)
			&& rect.width >= 0 && rect.height >= 0
			&& isValidX(rect.left + rect.width - 1)
			&& isValidY(rect.top + rect.height - 1);
	}

	/// @warning Check all indices by yourself
	[[nodiscard]] Tile& operator[] (sf::Vector3i position) noexcept {
		return tiles[position];
	}

	/// @warning Check all indices by yourself
	[[nodiscard]] const Tile& operator[] (sf::Vector3i position) const noexcept {
		return tiles[position];
	}

	/// Dungeon sizes in all axis
	sf::Vector3i shape() const noexcept {
		return tiles.shape();
	}

	/// Dungeon sizes in XY axis
	sf::IntRect horizontalBounds() const noexcept {
		return tiles.horizontalBounds();
	}

	/// Clears and creates new dungeon with given shape filled with given tile
	void assign(sf::Vector3i shape, Tile tile = Tile::WALL) {
		tiles.assign(shape, tile);
	}

	/// Returns at(position) destination if it's Tile::UP_STAIRS
	[[nodiscard]] std::optional<sf::Vector3i> upStairs(sf::Vector3i position) const {
		return getOptional(upStairs_, position);
	}

	/// Returns at(position) destination if it's Tile::DOWN_STAIRS
	[[nodiscard]] std::optional<sf::Vector3i> downStairs(sf::Vector3i position) const {
		return getOptional(downStairs_, position);
	}

	/// @brief Generates dungeon
	/// @details Generates dungeon with DungeonGenerator, then generate walls and stairs
	/// @param logger logger to log messages
	void generate(std::shared_ptr<spdlog::logger> logger);

	/// @brief Create stairs between position1 and position2
	/// @details Sets tiles and registers stairs in maps.
	/// Up or down stairs are choosen automatically by z coordinate.
	void addStairs(sf::Vector3i position1, sf::Vector3i position2);

	/// @brief Random tile position
	/// @details position distribution is uniform and independent for both dimensions
	[[nodiscard]] sf::Vector3i randomPositionAt(int level) const {
		return { std::uniform_int_distribution{ 0, shape().x - 1}(*randomEngine),
				 std::uniform_int_distribution{ 0, shape().y - 1}(*randomEngine),
		         level };
	}

	/// @brief 3D position of random tile satisfying pred
	/// @details randomPosition(engine) distrbution filtered by pred(tile)
	template <typename Pred>
		requires std::convertible_to<std::invoke_result_t<Pred, Tile>, bool>
	[[nodiscard]] sf::Vector3i randomPositionAt(int level, Pred&& pred) const {
		return randomPositionAt(level, [&pred](sf::Vector3i pos, const Dungeon& dungeon) {
			return std::invoke(pred, dungeon[pos]);
		});
	}

	/// @brief Random tile position (3D) at given level satisfying pred
	/// @details randomPosition(engine) distrbution filtered by pred(pos, *this)
	template <typename Pred>
		requires std::convertible_to<std::invoke_result_t<Pred, sf::Vector3i, const Dungeon&>, bool>
	[[nodiscard]] sf::Vector3i randomPositionAt(int level, Pred&& pred) const {
		sf::Vector3i pos;
		do {
			pos = randomPositionAt(level);
		} while (!std::invoke(pred, pos, *this));
		return pos;
	}

	/// Add bsp area. Used for debug area rendering
	void addArea(sf::IntRect area, int level) {
		areas_[level].push_back(area);
	}

	/// Get all bsp areas. Used for debug area rendering
	[[nodiscard]] std::span<const sf::IntRect> areas(int level) const noexcept {
		return areas_[level];
	}
private:
	Array3D<Tile> tiles;

	UnorderedMap<sf::Vector3i, sf::Vector3i> upStairs_;
	UnorderedMap<sf::Vector3i, sf::Vector3i> downStairs_;

	std::vector<std::vector<sf::IntRect>> areas_;

	std::unique_ptr<DungeonGenerator> generator_ = nullptr;
	RandomEngine* randomEngine = nullptr;

	void generateUpStairs(int fromLevel);
};

#endif