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

#ifndef PRIMITIVES_HPP_
#define PRIMITIVES_HPP_

#include "render/coords.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>

#include <string_view>

namespace render {
    void drawRect(sf::RenderTarget& target, sf::FloatRect rect,
        sf::Color fillColor, sf::Color outlineColor, float outlineThickness);

    inline void drawRect(sf::RenderTarget& target, sf::FloatRect rect, sf::Color color) {
        drawRect(target, rect, color, sf::Color::Transparent, 0.0f);
    }

    inline void drawInWorldRect(sf::RenderTarget& target, sf::IntRect rect,
        sf::Color fillColor, sf::Color outlineColor, float outlineThickness) {
        drawRect(target, {toScreen(rect.top, rect.left), toScreen(rect.width, rect.height)},
            fillColor, outlineColor, outlineThickness);
    }

    inline void drawInWorldRect(sf::RenderTarget& target, sf::IntRect rect, sf::Color color) {
        drawInWorldRect(target, rect, color, sf::Color::Transparent, 0.0f);
    }

    void drawSprite(sf::RenderTarget& target, sf::Vector2f screenPosition, sf::Vector2f origin, 
                    const sf::Texture& texture, double colorMod = 1.0, float scale = 1.0);

    void drawText(sf::RenderTarget& target, sf::Vector2f position, std::string_view string, 
                  const sf::Font& font, sf::Color color, int characterSize);
}

#endif