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

#include <SFML/Graphics/Text.hpp>

namespace render {
    Renderer::Renderer(std::shared_ptr<sf::RenderWindow> window_, std::shared_ptr<AssetManager> assets_) :
        assets_{std::move(assets_)},
        window{std::move(window_)} {}

    void Renderer::drawText(sf::Vector2f position, std::string_view string, sf::Color color, int characterSize) {
        sf::Text text;
        text.setString(sf::String::fromUtf8(string.begin(), string.end()));
        text.setFont(assets().font());
        text.setFillColor(color);
        text.setCharacterSize(characterSize);

        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.width / 2, textBounds.height / 2);
        text.setPosition(position);

        window->draw(text);
    }
}
