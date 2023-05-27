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

class Level{
public:
    bool isValidX(int x) const noexcept {
        return 0 <= x && x < shape().x;
    }

    bool isValidY(int y) const noexcept {
        return 0 <= y && y < shape().y;
    }

    bool isValidRect(sf::IntRect rect) const noexcept;

    // unsafe
    Tile& at(int x, int y) noexcept {
        TROTE_ASSERT(isValidX(x));
        TROTE_ASSERT(isValidY(y));
        return tiles[x * shape().y + y];
    }

    // unsafe
    const Tile& at(int x, int y) const noexcept {
        TROTE_ASSERT(isValidX(x));
        TROTE_ASSERT(isValidY(y));
        return tiles[x * shape().y + y];
    }

    // unsafe
    Tile& at(sf::Vector2i position) noexcept {
        return at(position.x, position.y);
    }

    // unsafe
    const Tile& at(sf::Vector2i position) const noexcept {
        return at(position.x, position.y);
    }

    sf::Vector2i shape() const noexcept {
        return shape_;
    }

    sf::IntRect bounds() const noexcept {
        return {{0, 0}, shape()};
    }

    void generateBlank(sf::Vector2i shape);
    void generateWalls();

    void addArea(sf::IntRect area) {
        areas_.push_back(area);
    }

    std::span<const sf::IntRect> areas() const noexcept {
        return areas_;
    }

    void addUpStairs(sf::Vector2i from, sf::Vector3i to) {
        upStairs_.insert_or_assign(from, to);
        at(from) = Tile::UP_STAIRS;
    }

    void addDownStairs(sf::Vector2i from, sf::Vector3i to) {
        downStairs_.insert_or_assign(from, to);
        at(from) = Tile::DOWN_STAIRS;
    }

    std::optional<sf::Vector3i> upStairs(sf::Vector2i position) {
        return getOptional(upStairs_, position);
    }

    std::optional<sf::Vector3i> downStairs(sf::Vector2i position) {
        return getOptional(downStairs_, position);
    }

    sf::Vector2i randomPosition(RandomEngine& engine) const {
        return { std::uniform_int_distribution{ 0, shape().x - 1 }(engine),
                 std::uniform_int_distribution{ 0, shape().y - 1 }(engine) };
    }

    template <typename Pred> 
        requires std::convertible_to<std::invoke_result_t<Pred, sf::Vector2i, const Level&>, bool>
    sf::Vector2i randomPosition(RandomEngine& engine, Pred&& pred) const {
        sf::Vector2i pos;
        do {
            pos = randomPosition(engine);
        } while (!std::invoke(pred, pos, *this));
        return pos;
    }

    template <typename Pred>
        requires std::convertible_to<std::invoke_result_t<Pred, Tile>, bool>
    sf::Vector2i randomPosition(RandomEngine& engine, Pred&& pred) const {
        return randomPosition(engine, [&pred](sf::Vector2i pos, const Level& level) {
            return std::invoke(pred, level.at(pos));
        });
    }
private:
    sf::Vector2i shape_;
    std::vector<Tile> tiles;

    UnorderedMap<sf::Vector2i, sf::Vector3i> upStairs_;
    UnorderedMap<sf::Vector2i, sf::Vector3i> downStairs_;

    std::vector<sf::IntRect> areas_;
};

#endif