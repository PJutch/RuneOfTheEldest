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

#ifndef VIEW_HPP
#define VIEW_HPP

/// @file View.hpp Utilities for sf::View creation

#include "coords.hpp"

#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

namespace render {
    /// @brief creates view
    /// Top left angle is at (0, 0)
    /// @param height Height of new view
    /// @param screenSize sides of the screen. Sides of the new view are proportional to them
    template <typename T>
    [[nodiscard]] sf::View createFullscreenView(float height,
        sf::Vector2<T> screenSize) noexcept {
        return sf::View{ {0, 0, height * screenSize.x / screenSize.y, height} };
    }

    /// @brief creates view. Uses user-defined center
    /// @param center Center of the new view
    /// @param height Height of new view
    /// @param screenSize sides of the screen. Sides of the new view are proportional to them
    template <typename T>
    [[nodiscard]] sf::View createFullscreenView(sf::Vector2f center,
        float height, sf::Vector2<T> screenSize) noexcept {
        return sf::View{ center, {height * screenSize.x / screenSize.y, height} };
    }

    inline sf::View worldScreenView(sf::Vector2f cameraPos, sf::Vector2u screenSize) noexcept {
        return createFullscreenView(toScreen(cameraPos), 512.f, screenSize);
    }
}

#endif