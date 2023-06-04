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

#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include "Tile.hpp"

#include "assert.hpp"
#include "random.hpp"
#include "geometry.hpp"
#include "Map.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <span>

/// Single level of the dungeon. Basically a 2D tile grid
class Level{
public:
    /// Checks if there are tiles with this x
    [[nodiscard]] bool isValidX(int x) const noexcept {
        return 0 <= x && x < shape().x;
    }

    /// Checks if there are tiles with this y
    [[nodiscard]] bool isValidY(int y) const noexcept {
        return 0 <= y && y < shape().y;
    }

    /// Checks if rect sides are >=0 and all tiles in it are exists
    [[nodiscard]] bool isValidRect(sf::IntRect rect) const noexcept;

    /// Access to individual tile at (x, y)
    /// @warning Check indices by yourself
    ///       \n You may use isValidX and isValidY or shape
    [[nodiscard]] Tile& at(int x, int y) noexcept {
        TROTE_ASSERT(isValidX(x));
        TROTE_ASSERT(isValidY(y));
        return tiles[x * shape().y + y];
    }

    /// Access to individual tile at (x, y)
    /// @warning Check indices by yourself
    ///       \n You may use isValidX and isValidY or shape
    [[nodiscard]] const Tile& at(int x, int y) const noexcept {
        TROTE_ASSERT(isValidX(x));
        TROTE_ASSERT(isValidY(y));
        return tiles[x * shape().y + y];
    }

    /// Access to individual tile at (position.x, position.y)
    /// @warning Check indices by yourself
    ///       \n You may use isValidX and isValidY or shape
    [[nodiscard]] Tile& at(sf::Vector2i position) noexcept {
        return at(position.x, position.y);
    }

    /// Access to individual tile at (position.x, position.y)
    /// @warning Check indices by yourself
    ///       \n You may use isValidX and isValidY or shape
    [[nodiscard]] const Tile& at(sf::Vector2i position) const noexcept {
        return at(position.x, position.y);
    }

    /// Size in both dimensions as vector (sizeX, sizeY)
    [[nodiscard]] sf::Vector2i shape() const noexcept {
        return shape_;
    }

    /// Rect containing all valid tiles
    [[nodiscard]] sf::IntRect bounds() const noexcept {
        return {{0, 0}, shape()};
    }

    /// Generates level filled with Tile::UNSEEN
    /// Shape of the new level
    void generateBlank(sf::Vector2i shape);

    /// Replaces Tile::UNSEEN adjanct to Tile::EMPTY with Tile::WALL 
    void generateWalls();

    /// Add bsp area. Used for debug area rendering
    void addArea(sf::IntRect area) {
        areas_.push_back(area);
    }

    /// Get all bsp areas. Used for debug area rendering
    [[nodiscard]] std::span<const sf::IntRect> areas() const noexcept {
        return areas_;
    }

    /// Random tile position
    /// @details position distribution is uniform and independent for both dimensions
    [[nodiscard]] sf::Vector2i randomPosition(RandomEngine& engine) const {
        return { std::uniform_int_distribution{ 0, shape().x - 1 }(engine),
                 std::uniform_int_distribution{ 0, shape().y - 1 }(engine) };
    }

    /// Random tile position satisfying pred
    /// @details randomPosition(engine) distrbution filtered by pred(pos, *this)
    template <typename Pred> 
        requires std::convertible_to<std::invoke_result_t<Pred, sf::Vector2i, const Level&>, bool>
    [[nodiscard]] sf::Vector2i randomPosition(RandomEngine& engine, Pred&& pred) const {
        sf::Vector2i pos;
        do {
            pos = randomPosition(engine);
        } while (!std::invoke(pred, pos, *this));
        return pos;
    }

    /// Position of random tile satisfying pred
    /// @details randomPosition(engine) distrbution filtered by pred(tile)
    template <typename Pred>
        requires std::convertible_to<std::invoke_result_t<Pred, Tile>, bool>
    [[nodiscard]] sf::Vector2i randomPosition(RandomEngine& engine, Pred&& pred) const {
        return randomPosition(engine, [&pred](sf::Vector2i pos, const Level& level) {
            return std::invoke(pred, level.at(pos));
        });
    }
private:
    sf::Vector2i shape_;
    std::vector<Tile> tiles;

    std::vector<sf::IntRect> areas_;
};

#endif