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

#include "Exception.hpp"

#include <SFML/System/Vector2.hpp>

#include <array>
#include <iterator>
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

    template <typename T>
    sf::Vector2<T> directionOf(sf::Vector2<T> v) {
        return {v.x > 0 ? 1 : (v.x < 0 ? -1 : 0),
                v.y > 0 ? 1 : (v.y < 0 ? -1 : 0)};
    }

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
    /// @warning Works only with vectors each components of which are -1, 0 or 1.
    template <typename T>
    sf::Vector2<T> turnDirection45Left(sf::Vector2<T> dir) {
        return directionOf(dir + turn90Left(dir));
    }

    /// @brief Turns vector 45 degrees right (clockwise)
    /// @warning SFML coordinate system where positive y direction is down not up
    /// @warning Works only with vectors each components of which are -1, 0 or 1.
    template <typename T>
    sf::Vector2<T> turnDirection45Right(sf::Vector2<T> dir) {
        return directionOf(dir + turn90Right(dir));
    }

    /// @brief Turns vector 135 degrees left (anticlockwise)
    /// @warning SFML coordinate system where positive y direction is down not up
    /// @warning Works only with vectors each components of which are -1, 0 or 1.
    template <typename T>
    sf::Vector2<T> turnDirection135Left(sf::Vector2<T> dir) {
        return -turnDirection45Right(dir);
    }

    /// @brief Turns vector 135 degrees right (clockwise)
    /// @warning SFML coordinate system where positive y direction is down not up
    /// @warning Works only with vectors each components of which are -1, 0 or 1.
    template <typename T>
    sf::Vector2<T> turnDirection135Right(sf::Vector2<T> dir) {
        return -turnDirection45Left(dir);
    }

    template <typename T>
    class NearestDirectionsRightBiasRange {
    public:
        NearestDirectionsRightBiasRange(sf::Vector2<T> direction_) :
            direction{direction_} {}

        auto begin() const noexcept {
            return *this;
        }

        auto end() const noexcept {
            return std::default_sentinel;
        }

        auto cbegin() const noexcept {
            return *this;
        }

        auto cend() const noexcept {
            return std::default_sentinel;
        }

        auto& operator++ () noexcept {
            ++current;
            return *this;
        }

        auto operator ++(int) const noexcept {
            auto copy = *this;
            ++current;
            return copy;
        }

        bool operator == (std::default_sentinel_t) const noexcept {
            return current >= 8;
        }

        class Exhausted : public RuntimeError {
            using RuntimeError::RuntimeError;
        };

        sf::Vector2<T> operator *() const {
            switch (current) {
            case 0: return direction;
            case 1: return turnDirection45Right(direction);
            case 2: return turnDirection45Left(direction);
            case 3: return turn90Right(direction);
            case 4: return turn90Left(direction);
            case 5: return turnDirection135Right(direction);
            case 6: return turnDirection135Left(direction);
            case 7: return -direction;
            default: throw Exhausted{"NearestDirectionsRightBiasRange has only 8 items"};
            }
        }
    private:
        sf::Vector2<T> direction;
        int current = 0;
    };

    template <typename T>
    class NearestDirectionsLeftBiasRange {
    public:
        NearestDirectionsLeftBiasRange(sf::Vector2<T> direction_) :
            direction{direction_} {}

        auto begin() const noexcept {
            return *this;
        }

        auto end() const noexcept {
            return std::default_sentinel;
        }

        auto cbegin() const noexcept {
            return *this;
        }

        auto cend() const noexcept {
            return std::default_sentinel;
        }

        auto& operator++ () noexcept {
            ++current;
            return *this;
        }

        auto operator ++(int) const noexcept {
            auto copy = *this;
            ++current;
            return copy;
        }

        bool operator == (std::default_sentinel_t) const noexcept {
            return current >= 8;
        }

        class Exhausted : public RuntimeError {
            using RuntimeError::RuntimeError;
        };

        sf::Vector2<T> operator *() const {
            switch (current) {
            case 0: return direction;
            case 1: return turnDirection45Left(direction);
            case 2: return turnDirection45Right(direction);
            case 3: return turn90Left(direction);
            case 4: return turn90Right(direction);
            case 5: return turnDirection135Left(direction);
            case 6: return turnDirection135Right(direction);
            case 7: return -direction;
            default: throw Exhausted{"NearestDirectionsLeftBiasRange has only 8 items"};
            }
        }
    private:
        sf::Vector2<T> direction;
        int current = 0;
    };
}

#endif