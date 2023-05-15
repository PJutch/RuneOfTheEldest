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

#include "World.hpp"

#include "geometry.hpp"
#include "log.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <array>
#include <string_view>
#include <filesystem>

class Renderer {
public:
    Renderer(std::shared_ptr<sf::RenderWindow> window, 
             std::shared_ptr<World> world, LoggerFactory& loggerFactory);

    void renderAreas(bool newRenderAreas = true) noexcept {
        renderAreas_ = newRenderAreas;
    }

    void handleEvent(sf::Event event);    
    void update(sf::Time elapsedTime);

    void draw();
private:
    bool renderAreas_ = false;

    float cameraSpeed = 300.f;
    sf::Vector2f cameraPosition_;
    int currentLevel = 0;
    sf::View levelView;

    inline const static sf::Vector2i tileSize{16, 16};
    std::array<sf::Texture, Level::totalTiles> tileTextures;

    std::shared_ptr<sf::RenderWindow> window;
    std::shared_ptr<World> world;

    std::shared_ptr<spdlog::logger> assetLogger;

    sf::Texture& tileTexture(Level::Tile tile) noexcept {
        return tileTextures[static_cast<int>(tile)];
    }

    const sf::Texture& tileTexture(Level::Tile tile) const noexcept {
        return tileTextures[static_cast<int>(tile)];
    }

    void fillTexture(sf::Texture& texture, 
                     sf::Vector2i size, sf::Color color) const noexcept {
        sf::Image image;
        image.create(size.x, size.y, color);
        texture.loadFromImage(image);
    }

    void loadTexture(sf::Texture& texture, std::string_view name, 
                     const std::filesystem::path& file) const;

    sf::Vector2f cameraPosition() const noexcept {
        return cameraPosition_;
    }

    void cameraPosition(sf::Vector2f newPosition) noexcept {
        cameraPosition_ = newPosition;
        levelView.setCenter(cameraPosition_);
    }

    sf::Vector2f toScreen(sf::Vector2i worldPoint) const noexcept {
        return sf::Vector2f(worldPoint.x * tileSize.x, 
                            worldPoint.y * tileSize.x);
    }

    void drawWorld() {
        draw((*world)[currentLevel]);
    }

    void draw(Level& level);
    void draw(Level::Tile tile, sf::Vector2i position);
    void drawAreas(Level& level);

    void drawInWorldRect(sf::IntRect rect, 
        sf::Color fillColor, sf::Color outlineColor, float outlineThickness);
};

#endif