/* This file is part of the Rune of the Eldest.
The Rune of the Eldest - Roguelike about the mage seeking for ancient knowledges
Copyright(C) 2023  PJutch

The Rune of the Eldest is free software : you can redistribute it and /or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

The Rune of the Eldest is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with the Rune of the Eldest.
If not, see < https://www.gnu.org/licenses/>. */

#ifndef COORDS_HPP_
#define COORDS_HPP_

#include "core/Position.hpp"
#include "util/geometry.hpp"

#include "View.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Mouse.hpp>

namespace render {
    inline const sf::Vector2i tileSize{16, 16};

    [[nodiscard]] inline sf::Vector2f toScreen(float worldX, float worldY) noexcept {
        return {worldX * tileSize.x, worldY * tileSize.y};
    }

    [[nodiscard]] inline sf::Vector2f toScreen(sf::Vector2f worldVector) noexcept {
        return toScreen(worldVector.x, worldVector.y);
    }

    [[nodiscard]] inline sf::Vector2f toScreen(sf::Vector2i worldVector) noexcept {
        return toScreen(util::geometry_cast<float>(worldVector));
    }

    [[nodiscard]] inline sf::Vector2f toScreen(int worldX, int worldY) noexcept {
        return toScreen(sf::Vector2i{worldX, worldY});
    }

    /// Converts pos on screen to tile pos 
    [[nodiscard]] inline sf::Vector2f fromScreen(sf::Vector2f pos) {
        return {pos.x / tileSize.x, pos.y / tileSize.y};
    }

    /// Gets mouse pos in tile space
    [[nodiscard]] inline sf::Vector2i mouseTile(sf::Vector2i mousePixel, core::Position<float> cameraPos, 
            const sf::RenderTarget& target) {
        auto [mouseX, mouseY] = target.mapPixelToCoords(mousePixel, 
            createFullscreenView(toScreen(cameraPos.xy()), 512.f, target.getSize()));
        return util::geometry_cast<int>(fromScreen({mouseX, mouseY}));
    }
}
#endif