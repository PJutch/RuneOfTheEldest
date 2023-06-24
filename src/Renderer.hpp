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
#include "AssetManager.hpp"
#include "PlayerMap.hpp"

#include "World.hpp"
#include "AiState.hpp"
#include "Player.hpp"
#include "Goblin.hpp"

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
             std::shared_ptr<PlayerMap> playerMap,
             std::shared_ptr<sf::RenderWindow> window, 
             std::shared_ptr<World> world,
             std::shared_ptr<Player> player,
             std::unique_ptr<AssetManager> assets);

    /// If true bsp areas created by dungeon generation are rendered
    void renderAreas(bool newRenderAreas = true) noexcept {
        renderAreas_ = newRenderAreas;
    }

    /// Updates Renderer components such as Camera
    void update(sf::Time elapsedTime);

    /// Renders game world
    void draw();

    /// Renders player
    void draw(const Player::DrawMemento& player);

    /// Renders goblin
    void draw(const Goblin::DrawMemento& goblin);

    /// Renders "You died screen"
    void drawDeathScreen();
private:
    bool renderAreas_ = false;

    std::shared_ptr<Camera> camera;
    std::unique_ptr<AssetManager> assets;
    std::shared_ptr<PlayerMap> playerMap;

    std::shared_ptr<World> world;
    std::shared_ptr<Player> player;

    std::shared_ptr<sf::RenderWindow> window;

    [[nodiscard]] sf::Vector2f toScreen(sf::Vector2i worldVector) const noexcept {
        return toScreen(worldVector.x, worldVector.y);
    }

    [[nodiscard]] sf::Vector2f toScreen(sf::Vector2f worldVector) const noexcept {
        return toScreen(worldVector.x, worldVector.y);
    }

    [[nodiscard]] sf::Vector2f toScreen(float worldX, float worldY) const noexcept {
        auto [tileX, tileY] = assets->tileSize();
        return { worldX * tileX, worldY * tileY };
    }

    void worldScreenView() noexcept {
        auto cameraPos = toScreen(camera->position().xy());
        window->setView(createFullscreenView(cameraPos, 512, window->getSize()));
    }

    void drawWorld();
    void draw(const Level& level, int z);
    void drawAreas(const Level& level);

    void drawHpBar(sf::Vector3i position, double hp, double maxHp);

    void drawSprite(sf::Vector3i position, const sf::Texture& texture, double colorMod = 1.0);
    void drawSprite(sf::Vector2i position, const sf::Texture& texture, double colorMod = 1.0);

    void drawRect(sf::FloatRect rect,
        sf::Color fillColor, sf::Color outlineColor, float outlineThickness);

    void drawRect(sf::FloatRect rect, sf::Color color) {
        drawRect(rect, color, sf::Color::Transparent, 0.0f);
    }

    void drawInWorldRect(sf::IntRect rect, 
        sf::Color fillColor, sf::Color outlineColor, float outlineThickness);

    void drawInWorldRect(sf::IntRect rect, sf::Color color) {
        drawInWorldRect(rect, color, sf::Color::Transparent, 0.0f);
    }

    void drawText(sf::Vector2f position, const std::string& text, sf::Color color, int characterSize);
};

#endif