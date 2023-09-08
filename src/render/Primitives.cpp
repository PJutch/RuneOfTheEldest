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

#include "Primitives.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace render {
    void drawRect(sf::RenderTarget& target, sf::FloatRect rect, 
                  sf::Color fillColor, sf::Color outlineColor, float outlineThickness) {
        sf::RectangleShape rectShape{{rect.width, rect.height}};
        rectShape.setPosition(rect.left, rect.top);

        rectShape.setFillColor(fillColor);
        rectShape.setOutlineColor(outlineColor);
        rectShape.setOutlineThickness(outlineThickness);

        target.draw(rectShape);
    }

    void drawSprite(sf::RenderTarget& target, sf::Vector2f screenPosition, sf::Vector2f origin, 
                    const sf::Texture& texture, double colorMod, float scale) {
        sf::Sprite sprite;
        sprite.setTexture(texture);

        auto spriteColor = static_cast<sf::Uint8>(colorMod * 255);
        sprite.setColor({spriteColor , spriteColor , spriteColor});

        sprite.setScale(scale, scale);

        sprite.setPosition(screenPosition);
        sprite.setOrigin(origin);

        target.draw(sprite);
    }
}
