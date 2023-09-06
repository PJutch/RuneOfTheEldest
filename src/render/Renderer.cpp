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

#include "Renderer.hpp"

#include "core/World.hpp"

#include "util/raycast.hpp"
#include "util/geometry.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace render {
    Renderer::Renderer(std::shared_ptr<Camera> camera,
            std::shared_ptr<PlayerMap> newPlayerMap,
            std::shared_ptr<sf::RenderWindow> window_,
            std::shared_ptr<core::World> world_,
            std::shared_ptr<core::XpManager> xpManager_,
            std::shared_ptr<util::Raycaster> raycaster_,
            std::shared_ptr<AssetManager> assets_) :
        camera_{std::move(camera)}, assets_{std::move(assets_)}, playerMap_{std::move(newPlayerMap)},
        world{std::move(world_)}, xpManager{std::move(xpManager_)},
        raycaster_{std::move(raycaster_)}, window{std::move(window_)} {}

    void Renderer::drawRect(sf::FloatRect rect, sf::Color fillColor, sf::Color outlineColor, float outlineThickness) {
        sf::RectangleShape rectShape{{rect.width, rect.height}};
        rectShape.setPosition(rect.left, rect.top);

        rectShape.setFillColor(fillColor);
        rectShape.setOutlineColor(outlineColor);
        rectShape.setOutlineThickness(outlineThickness);

        window->draw(rectShape);
    }

    void Renderer::drawSprite(sf::Vector2f screenPosition, sf::Vector2f origin, const sf::Texture& texture, 
                              double colorMod, float scale) {
        sf::Sprite sprite;
        sprite.setTexture(texture);

        auto spriteColor = static_cast<sf::Uint8>(colorMod * 255);
        sprite.setColor({spriteColor , spriteColor , spriteColor});

        sprite.setScale(scale, scale);

        sprite.setPosition(screenPosition);
        sprite.setOrigin(origin);

        window->draw(sprite);
    }

    void Renderer::drawText(sf::Vector2f position, std::string_view string, sf::Color color, int characterSize) {
        sf::Text text;
        text.setString(sf::String::fromUtf8(string.begin(), string.end()));
        text.setFont(assetsRef().font());
        text.setFillColor(color);
        text.setCharacterSize(characterSize);

        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.width / 2, textBounds.height / 2);
        text.setPosition(position);

        window->draw(text);
    }
}
