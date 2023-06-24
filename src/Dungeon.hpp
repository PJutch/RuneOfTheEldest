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
#include "Level.hpp"
#include "Tile.hpp"

#include "log.hpp"
#include "random.hpp"
#include "Map.hpp"

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

	/// @brief Access to individual level
	/// @warning Check level index by yourself
	///       \n You may use size
	[[nodiscard]] Level& operator[] (int level) {
		return levels[level];
	}

	/// @brief Access to individual level
	/// @warning Check level index by yourself
	///       \n You may use size
	[[nodiscard]] const Level& operator[] (int level) const {
		return levels[level];
	}

	/// @brief Access to individual tile on level level at (x, y)
	/// @warning Check all indices by yourself
	[[nodiscard]] Tile& at(int x, int y, int level) noexcept {
		return (*this)[level].at(x, y);
	}

	/// @brief Access to individual tile on level level at (x, y)
	/// @warning Check all indices by yourself
	[[nodiscard]] const Tile& at(int x, int y, int level) const noexcept {
		return (*this)[level].at(x, y);
	}

	/// @brief Access to individual tile on level level at (x, y)
	/// @warning Check all indices by yourself
	[[nodiscard]] Tile& at(sf::Vector2i position, int level) noexcept {
		return at(position.x, position.y, level);
	}

	/// @brief Access to individual tile on level level at (x, y)
	/// @warning Check all indices by yourself
	[[nodiscard]] const Tile& at(sf::Vector2i position, int level) const noexcept {
		return at(position.x, position.y, level);
	}

	/// @brief Access to individual tile on level z at (x, y)
	/// @warning Check all indices by yourself
	[[nodiscard]] Tile& at(sf::Vector3i position) noexcept {
		return at(position.x, position.y, position.z);
	}

	/// @brief Access to individual tile on level z at (x, y)
	/// @warning Check all indices by yourself
	[[nodiscard]] const Tile& at(sf::Vector3i position) const noexcept {
		return at(position.x, position.y, position.z);
	}

	/// Level count
	[[nodiscard]] int size() const noexcept {
		return levels.size();
	}

	/// Creates or deletes last levels to make size() == newSize
	void resize(int newSize) noexcept {
		levels.resize(newSize);
	}

	/// Checks if position is valid tile position
	[[nodiscard]] bool isValidPosition(sf::Vector3i position) const noexcept {
		return 0 <= position.z && position.z < size()
			&& (*this)[position.z].isValidX(position.x)
			&& (*this)[position.z].isValidY(position.y);
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
		return { std::uniform_int_distribution{ 0, (*this)[level].shape().x - 1 }(*randomEngine),
				 std::uniform_int_distribution{ 0, (*this)[level].shape().y - 1 }(*randomEngine),
		         level };
	}

	/// @brief 3D position of random tile satisfying pred
	/// @details randomPosition(engine) distrbution filtered by pred(tile)
	template <typename Pred>
		requires std::convertible_to<std::invoke_result_t<Pred, Tile>, bool>
	[[nodiscard]] sf::Vector3i randomPositionAt(int level, Pred&& pred) const {
		return randomPositionAt(level, [&pred](sf::Vector3i pos, const Dungeon& dungeon) {
			return std::invoke(pred, dungeon.at(pos));
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
private:
	std::vector<Level> levels;
	UnorderedMap<sf::Vector3i, sf::Vector3i> upStairs_;
	UnorderedMap<sf::Vector3i, sf::Vector3i> downStairs_;

	std::unique_ptr<DungeonGenerator> generator_ = nullptr;
	RandomEngine* randomEngine = nullptr;
	void generateUpStairs(int fromLevel);
};

#endif