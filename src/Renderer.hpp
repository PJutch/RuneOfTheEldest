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

#include "Camera/Camera.hpp"

#include "View.hpp"

#include "World.hpp"
#include "Player.hpp"

#include "geometry.hpp"
#include "log.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <array>
#include <string_view>
#include <filesystem>

/// Renders game world
class Renderer {
public:
    /// Creates renderer and loads textures
    Renderer(std::shared_ptr<Camera> camera,
             std::shared_ptr<sf::RenderWindow> window, 
             std::shared_ptr<World> world_, std::shared_ptr<Player> player_, 
             LoggerFactory& loggerFactory);

    /// If true bsp areas created by dungeon generation are rendered
    void renderAreas(bool newRenderAreas = true) noexcept {
        renderAreas_ = newRenderAreas;
    }

    /// Renders game world
    void draw();
private:
    bool renderAreas_ = false;

    std::shared_ptr<Camera> camera;

    inline const static sf::Vector2i tileSize{16, 16};
    std::array<sf::Texture, totalTiles> tileTextures;

    sf::Texture playerTexture;

    std::shared_ptr<World> world;
    std::shared_ptr<Player> player;

    std::shared_ptr<sf::RenderWindow> window;

    std::shared_ptr<spdlog::logger> assetLogger;

    sf::Texture& tileTexture(Tile tile) noexcept {
        return tileTextures[static_cast<int>(tile)];
    }

    const sf::Texture& tileTexture(Tile tile) const noexcept {
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

    sf::Vector2f toScreen(sf::Vector2i worldVector) const noexcept {
        return toScreen(worldVector.x, worldVector.y);
    }

    sf::Vector2f toScreen(sf::Vector2f worldVector) const noexcept {
        return toScreen(worldVector.x, worldVector.y);
    }

    sf::Vector2f toScreen(float worldX, float worldY) const noexcept {
        return { worldX * tileSize.x, worldY * tileSize.y };
    }

    void worldScreenView() noexcept {
        auto cameraPos = toScreen(camera->position());
        window->setView(createFullscreenView(cameraPos, 512, window->getSize()));
    }

    void drawWorld() {
        draw((*world)[camera->level()]);
    }

    void drawPlayer();
    void draw(Level& level);
    void draw(Tile tile, sf::Vector2i position);
    void drawAreas(Level& level);

    void drawInWorldRect(sf::IntRect rect, 
        sf::Color fillColor, sf::Color outlineColor, float outlineThickness);
};

#endif