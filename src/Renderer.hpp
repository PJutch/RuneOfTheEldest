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

#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include "Level.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <array>

class Renderer {
public:
    Renderer(std::shared_ptr<sf::RenderWindow> window);

    void draw(const Level& level);
    void draw(Level::Tile tile, sf::Vector2i position);
private:
    std::shared_ptr<sf::RenderWindow> window_;

    sf::View levelView;

    inline const static sf::Vector2i tileSize{16, 16};
    std::array<sf::Texture, Level::totalTiles> tileTextures;

    sf::RenderWindow& window() noexcept {
        return *window_;
    }

    const sf::RenderWindow& window() const noexcept {
        return *window_;
    }

    sf::Texture& tileTexture(Level::Tile tile) noexcept {
        return tileTextures[static_cast<int>(tile)];
    }

    const sf::Texture& tileTexture(Level::Tile tile) const noexcept {
        return tileTextures[static_cast<int>(tile)];
    }
};

#endif