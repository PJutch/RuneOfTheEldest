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

#ifndef GEOMETRY_HPP_
#define GEOMETRY_HPP_

/// @file geometry.hpp Geometry related utilities

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <array>
#include <type_traits>
#include <cmath>

namespace util {
    /// Cast sf::Vector2<U> to sf::Vector2<T> using static_cast
    template <typename T, typename U>
    [[nodiscard]] sf::Vector2<T> geometry_cast(sf::Vector2<U> vec) noexcept {
        return { static_cast<T>(vec.x), static_cast<T>(vec.y) };
    }

    /// Cast sf::Vector3<U> to sf::Vector3<T> using static_cast
    template <typename T, typename U>
    [[nodiscard]] sf::Vector3<T> geometry_cast(sf::Vector3<U> vec) noexcept {
        return { static_cast<T>(vec.x), static_cast<T>(vec.y), static_cast<T>(vec.z) };
    }

    /// Add deltaX to vec.x
    /// @returns result as new vector
    template <typename T>
    [[nodiscard]] sf::Vector2<T> addX(sf::Vector2<T> vec, T deltaX) noexcept {
        return { vec.x + deltaX, vec.y };
    }

    /// Substract deltaX from vec.x
    /// @returns result as new vector
    template <typename T>
    [[nodiscard]] sf::Vector2<T> subX(sf::Vector2<T> vec, T deltaX) noexcept {
        return { vec.x - deltaX, vec.y };
    }

    /// Add deltaY to vec.y
    /// @returns result as new vector
    template <typename T>
    [[nodiscard]] sf::Vector2<T> addY(sf::Vector2<T> vec, T deltaY) noexcept {
        return { vec.x, vec.y + deltaY };
    }

    /// Substract deltaY from vec.y
    /// @returns result as new vector
    template <typename T>
    [[nodiscard]] sf::Vector2<T> subY(sf::Vector2<T> vec, T deltaY) noexcept {
        return { vec.x, vec.y - deltaY };
    }

    /// Add z component to 2D vector
    /// @returns result as new 3D vector
    template <typename T>
    [[nodiscard]] sf::Vector3<T> make3D(sf::Vector2<T> xy, T z) noexcept {
        return { xy.x, xy.y, z };
    }

    /// Gets x and y components from 3D vector
    /// @returns result as new 2D vector
    template <typename T>
    [[nodiscard]] sf::Vector2<T> getXY(sf::Vector3<T> vec) noexcept {
        return { vec.x, vec.y };
    }

    /// Gets relative position of bottom middle of the rect with given size 
    template <typename T>
    [[nodiscard]] sf::Vector2<T> bottomMiddle(sf::Vector2<T> size) noexcept {
        return { size.x / static_cast<T>(2), size.y};
    }

    /// Gets relative position of bottom left corner of the rect with given size 
    template <typename T>
    [[nodiscard]] sf::Vector2<T> bottomLeft(sf::Vector2<T> size) noexcept {
        return { T{}, size.y };
    }

    /// Gets relative position of top right corner of the rect with given size 
    template <typename T>
    [[nodiscard]] sf::Vector2<T> topRight(sf::Vector2<T> size) noexcept {
        return { size.x, T{} };
    }
}

namespace detail {
    template <std::size_t index, typename T>
    struct vector2_member_st;

    template <typename T>
    struct vector2_member_st<0, T>  {
        inline static constexpr T sf::Vector2<T>::* value = &sf::Vector2<T>::x;
    };

    template <typename T>
    struct vector2_member_st<1, T> {
        inline static constexpr T sf::Vector2<T>::* value = &sf::Vector2<T>::y;
    };

    template <std::size_t index, typename T>
    inline static constexpr T sf::Vector2<T>::* vector2_member = vector2_member_st<index, T>::value;

    template <std::size_t index, typename T>
    struct vector3_member_st;

    template <typename T>
    struct vector3_member_st<0, T> {
        inline static constexpr T sf::Vector3<T>::* value = &sf::Vector3<T>::x;
    };

    template <typename T>
    struct vector3_member_st<1, T> {
        inline static constexpr T sf::Vector3<T>::* value = &sf::Vector3<T>::y;
    };

    template <typename T>
    struct vector3_member_st<2, T> {
        inline static constexpr T sf::Vector3<T>::* value = &sf::Vector3<T>::z;
    };

    template <std::size_t index, typename T>
    inline static constexpr T sf::Vector3<T>::* vector3_member = vector3_member_st<index, T>::value;
}

namespace sf {
    /// @brief Tuplelike get for sf::Vector2<T>
    /// x is 0, y is 1
    template <std::size_t index, typename T>
    constexpr T& get(Vector2<T>& vec) noexcept {
        return vec.*detail::vector2_member<index, T>;
    }

    /// @brief Tuplelike get for sf::Vector2<T>
    /// x is 0, y is 1
    template <std::size_t index, typename T>
    T&& get(Vector2<T>&& vec) noexcept {
        return std::move(vec.*detail::vector2_member<index, T>);
    }

    /// @brief Tuplelike get for sf::Vector2<T>
    /// x is 0, y is 1
    template <std::size_t index, typename T>
    constexpr const T& get(const Vector2<T>& vec) noexcept  {
        return vec.*detail::vector2_member<index, T>;
    }

    /// @brief Tuplelike get for sf::Vector3<T>
    /// x is 0, y is 1, z is 2
    template <std::size_t index, typename T>
    constexpr T& get(Vector3<T>& vec) noexcept {
        return vec.*detail::vector3_member<index, T>;
    }

    /// @brief Tuplelike get for sf::Vector3<T>
    /// x is 0, y is 1, z is 2
    template <std::size_t index, typename T>
    T&& get(Vector3<T>&& vec) noexcept {
        return std::move(vec.*detail::vector3_member<index, T>);
    }

    /// @brief Tuplelike get for sf::Vector3<T>
    /// x is 0, y is 1, z is 2
    template <std::size_t index, typename T>
    constexpr const T& get(const Vector3<T>& vec) noexcept {
        return vec.*detail::vector3_member<index, T>;
    }
}

namespace std {
    /// @brief std::tuple_size for sf::Vector2<T>
    /// @returns always 2
    template <typename T>
    struct tuple_size<sf::Vector2<T>>
        : std::integral_constant<std::size_t, 2> { };

    /// @brief std::tuple_element for sf::Vector2<T>
    /// @returns always T
    template<std::size_t index, class T>
    struct tuple_element<index, sf::Vector2<T>> {
        using type = T;
    };

    /// @brief std::tuple_size for sf::Vector3<T>
    /// @returns always 3
    template <typename T>
    struct tuple_size<sf::Vector3<T>>
        : std::integral_constant<std::size_t, 3> { };

    /// @brief std::tuple_element for sf::Vector3<T>
    /// @returns always T
    template<std::size_t index, class T>
    struct tuple_element<index, sf::Vector3<T>> {
        using type = T;
    };
}

namespace util {
    /// Moves top left angle keeping bottom right angle in place
    /// @param rect initial rect
    /// @param extension offset to move angle
    /// If positive, rectangle is extended
    /// @returns result as new rect
    template <typename T>
    [[nodiscard]] sf::Rect<T> extendTopLeft(sf::Rect<T> rect,
        sf::Vector2<T> extension) noexcept {
        return { rect.left - extension.x, rect.top - extension.y,
                rect.width + extension.x, rect.height + extension.y };
    }

    /// Moves bottom right angle keeping top left angle in place
    /// @param rect initial rect
    /// @param extension offset to move angle
    /// If positive, rectangle is extended
    /// @returns result as new rect
    template <typename T>
    [[nodiscard]] sf::Rect<T> extendBottomRight(sf::Rect<T> rect,
        sf::Vector2<T> extension) noexcept {
        return { rect.left, rect.top,
                rect.width + extension.x, rect.height + extension.y };
    }

    /// Moves bottom right angle keeping top left angle in place
    /// @param rect initial rect
    /// @param shrink offset to move angle
    /// If positive, rectangle is shrinked
    /// @returns result as new rect
    template <typename T>
    [[nodiscard]] sf::Rect<T> shrinkTopLeft(sf::Rect<T> rect,
        sf::Vector2<T> shrink) noexcept {
        return { rect.left + shrink.x, rect.top + shrink.y,
                rect.width - shrink.x, rect.height - shrink.y };
    }

    /// Moves bottom right angle keeping top left angle in place
    /// @param rect initial rect
    /// @param shrink offset to move angle
    /// If positive, rectangle is shrinked
    /// @returns result as new rect
    template <typename T>
    [[nodiscard]] sf::Rect<T> shrinkBottomRight(sf::Rect<T> rect,
        sf::Vector2<T> shrink) noexcept {
        return { rect.left, rect.top,
                rect.width - shrink.x, rect.height - shrink.y };
    }

    template <typename T>
    inline static const std::array<sf::Vector2<T>, 9> numpadDirections{
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
    inline static const std::array<sf::Vector2<T>, 8> directions{
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
        return { dir.y,  -dir.x };
    }

    /// @brief Turns vector 90 degrees right (clockwise)
    /// @warning SFML coordinate system where positive y direction is down not up
    template <typename T>
    sf::Vector2<T> turn90Right(sf::Vector2<T> dir) {
        return { -dir.y, dir.x };
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

    /// Computes uniform (Chebyshev, supremum, max, infinity) norm of 3D vector
    template <typename T>
    T uniformNorm(sf::Vector3<T> vec) noexcept {
        return std::max(std::max(std::abs(vec.x), std::abs(vec.y)), std::abs(vec.z));
    }

    /// Computes uniform (Chebyshev, supremum, max, infinity) norm of 2D vector
    template <typename T>
    T uniformNorm(sf::Vector2<T> vec) noexcept {
        return std::max(std::abs(vec.x), std::abs(vec.y));
    }

    /// Computes uniform (Chebyshev, supremum, max, infinity) distance between 3D points
    template <typename T>
    T uniformDistance(sf::Vector3<T> from, sf::Vector3<T> to) noexcept {
        return uniformNorm(to - from);
    }

    /// Computes uniform (Chebyshev, supremum, max, infinity) distance between 2D points
    template <typename T>
    T uniformDistance(sf::Vector2<T> from, sf::Vector2<T> to) noexcept {
        return uniformNorm(to - from);
    }

    /// Computes euclidian norm of 2D vector
    template <typename T>
    auto norm(sf::Vector2<T> vec) noexcept {
        return std::hypot(vec.x, vec.y);
    }

    /// Computes euclidian distance between 2D points
    template <typename T>
    auto distance(sf::Vector2<T> from, sf::Vector2<T> to) noexcept {
        return norm(to - from);
    }
}

#endif