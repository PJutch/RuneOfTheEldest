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

#ifndef DIRECTION_HPP_
#define DIRECTION_HPP_

/// @file Direction.hpp Utility for directions (2D vectors with values in {-1, 0, 1})

#include <SFML/System/Vector2.hpp>

#include <array>
#include <type_traits>
#include <cmath>

namespace util {
    template <typename T>
    inline static const std::array<sf::Vector2<T>, 9> directions{
        sf::Vector2<T>(-1,  1),
        sf::Vector2<T>(0,  1),
        sf::Vector2<T>(1,  1),
        sf::Vector2<T>(-1,  0),
        sf::Vector2<T>(0,  0),
        sf::Vector2<T>(1,  0),
        sf::Vector2<T>(-1, -1),
        sf::Vector2<T>(0, -1),
        sf::Vector2<T>(1, -1)
    };

    template <typename T>
    inline static const std::array<sf::Vector2<T>, 8> nonzeroDirections{
        sf::Vector2<T>(-1,  1),
        sf::Vector2<T>(0,  1),
        sf::Vector2<T>(1,  1),
        sf::Vector2<T>(-1,  0),
        sf::Vector2<T>(1,  0),
        sf::Vector2<T>(-1, -1),
        sf::Vector2<T>(0, -1),
        sf::Vector2<T>(1, -1)
    };

    /// @brief Turns vector 90 degrees left (anticlockwise)
    /// @warning SFML coordinate system where positive y direction is down not up
    template <typename T>
    sf::Vector2<T> turn90Left(sf::Vector2<T> dir) {
        return {dir.y,  -dir.x};
    }

    /// @brief Turns vector 90 degrees right (clockwise)
    /// @warning SFML coordinate system where positive y direction is down not up
    template <typename T>
    sf::Vector2<T> turn90Right(sf::Vector2<T> dir) {
        return {-dir.y, dir.x};
    }

    /// @brief Turns vector 45 degrees left (anticlockwise)
    /// @warning SFML coordinate system where positive y direction is down not up
    /// @warning Works only with vectos each components of which are -1, 0 or 1.
    template <typename T>
    sf::Vector2<T> turnDirection45Left(sf::Vector2<T> dir) {
        return dir + turn90Left(dir);
    }

    /// @brief Turns vector 45 degrees right (clockwise)
    /// @warning SFML coordinate system where positive y direction is down not up
    /// @warning Works only with vectos each components of which are -1, 0 or 1.
    template <typename T>
    sf::Vector2<T> turnDirection45Right(sf::Vector2<T> dir) {
        return dir + turn90Right(dir);
    }
}

#endif