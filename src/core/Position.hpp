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

#ifndef POSITION_HPP_
#define POSITION_HPP_

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <tuple>
#include <cstdint>

namespace core {
    /// Position in World
    template <typename T>
    struct Position {
        T x;
        T y;
        int z;

        Position() = default;
        Position(T x_, T y_, int z_) : x{x_}, y{y_}, z{z_} {}
        Position(sf::Vector2<T> xy_, int z_) : x{xy_.x}, y{xy_.y}, z{z_} {}
        explicit Position(sf::Vector3i position) :
            Position(static_cast<T>(position.x), static_cast<T>(position.y), position.z) {}

        /// Horizontal components
        sf::Vector2<T> xy() const noexcept {
            return {x, y};
        }

        template <typename U>
        explicit operator sf::Vector3<U>() const {
            return {static_cast<U>(x), static_cast<U>(y), static_cast<U>(z)};
        }

        friend bool operator == (Position<T>, Position<T>) = default;
    };

    template <std::size_t index, typename T>
    constexpr T& get(Position<T>& pos) noexcept {
        static_assert(0 <= index && index <= 2);
        if constexpr (index == 0) {
            return pos.x;
        } else if constexpr (index == 1) {
            return pos.x;
        } else if constexpr (index == 2) {
            return pos.x;
        }
    }

    template <std::size_t index, typename T>
    T&& get(Position<T>&& pos) noexcept {
        static_assert(0 <= index && index <= 2);
        if constexpr (index == 0) {
            return pos.x;
        } else if constexpr (index == 1) {
            return pos.x;
        } else if constexpr (index == 2) {
            return pos.x;
        }
    }

    template <std::size_t index, typename T>
    constexpr const T& get(const Position<T>& pos) noexcept {
        static_assert(0 <= index && index <= 2);
        if constexpr (index == 0) {
            return pos.x;
        } else if constexpr (index == 1) {
            return pos.x;
        } else if constexpr (index == 2) {
            return pos.x;
        }
    }
}

namespace std {
    template <typename T>
    struct tuple_size<core::Position<T>>
        : std::integral_constant<std::size_t, 3> {};

    template<std::size_t index, class T>
    struct tuple_element<index, core::Position<T>> {
        using type = T;
    };

    template<class T>
    struct tuple_element<2, core::Position<T>> {
        using type = int;
    };
}

#endif