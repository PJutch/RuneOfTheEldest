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

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

template <typename T, typename U>
sf::Vector2<T> geometry_cast(sf::Vector2<U> vec) noexcept {
    return {static_cast<T>(vec.x), static_cast<T>(vec.y)};
}

template <typename T>
sf::Vector2<T> addX(sf::Vector2<T> vec, T deltaX) noexcept {
    return {vec.x + deltaX, vec.y};
}

template <typename T>
sf::Vector2<T> subX(sf::Vector2<T> vec, T deltaX) noexcept {
    return {vec.x - deltaX, vec.y};
}

template <typename T>
sf::Vector2<T> addY(sf::Vector2<T> vec, T deltaY) noexcept {
    return {vec.x, vec.y + deltaY};
}

template <typename T>
sf::Vector2<T> subY(sf::Vector2<T> vec, T deltaY) noexcept {
    return {vec.x, vec.y - deltaY};
}

template <typename T>
sf::Rect<T> extendTopLeft(sf::Rect<T> rect, 
                          sf::Vector2<T> extension) noexcept {
    return {rect.left  - extension.x, rect.top    - extension.y, 
            rect.width + extension.x, rect.height + extension.y};
}

template <typename T>
sf::Rect<T> extendBottomRight(sf::Rect<T> rect, 
                              sf::Vector2<T> extension) noexcept {
    return {rect.left, rect.top, 
            rect.width + extension.x, rect.height + extension.y};
}

template <typename T>
sf::Rect<T> shrinkTopLeft(sf::Rect<T> rect, 
                          sf::Vector2<T> shrink) noexcept {
    return {rect.left  + shrink.x, rect.top    + shrink.y, 
            rect.width - shrink.x, rect.height - shrink.y};
}

template <typename T>
sf::Rect<T> shrinkBottomRight(sf::Rect<T> rect, 
                              sf::Vector2<T> shrink) noexcept {
    return {rect.left, rect.top, 
            rect.width - shrink.x, rect.height - shrink.y};
}

#endif
