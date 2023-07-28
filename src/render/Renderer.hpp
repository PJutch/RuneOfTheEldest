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

#include "Camera.hpp"
#include "View.hpp"
#include "AssetManager.hpp"
#include "PlayerMap.hpp"

#include "core/fwd.hpp"
#include "core/AiState.hpp"

#include "util/geometry.hpp"
#include "util/log.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <array>
#include <string_view>
#include <filesystem>

namespace render {
    /// Renders game world
    class Renderer {
    public:
        /// Creates renderer and loads textures
        Renderer(std::shared_ptr<Camera> camera,
            std::shared_ptr<PlayerMap> playerMap,
            std::shared_ptr<sf::RenderWindow> window,
            std::shared_ptr<core::World> world,
            std::shared_ptr<util::Raycaster> raycaster,
            std::shared_ptr<AssetManager> assets);

        /// If true bsp areas created by dungeon generation are rendered
        void renderAreas(bool newRenderAreas = true) noexcept {
            renderAreas_ = newRenderAreas;
        }

        PlayerMap& playerMap() noexcept {
            return *playerMap_;
        }

        const PlayerMap& playerMap() const noexcept {
            return *playerMap_;
        }

        /// @brief Passes input Event to Renderer components such as Camera
        /// @returns true if Event shouldn't be passed to other objects
        bool handleEvent(sf::Event event) {
            camera->handleEvent(event);
            return camera->shouldStealControl();
        }

        /// Updates Renderer components such as Camera
        void update(sf::Time elapsedTime);

        /// Notifies Renderer components such as PlayerMap about world generation
        void onGenerate() {
            playerMap_->onGenerate();
            camera->reset();
        }

        /// Renders game world
        void draw();

        /// Renders "You died screen"
        void drawDeathScreen();

        std::shared_ptr<AssetManager> assets() const noexcept {
            return assets_;
        }
    private:
        bool renderAreas_ = false;

        std::shared_ptr<Camera> camera;
        std::shared_ptr<AssetManager> assets_;
        std::shared_ptr<PlayerMap> playerMap_;

        std::shared_ptr<core::World> world;
        std::shared_ptr<util::Raycaster> raycaster;

        std::shared_ptr<sf::RenderWindow> window;

        [[nodiscard]] sf::Vector2f toScreen(sf::Vector2i worldVector) const noexcept {
            return toScreen(util::geometry_cast<float>(worldVector));
        }

        [[nodiscard]] sf::Vector2f toScreen(sf::Vector2f worldVector) const noexcept {
            return toScreen(worldVector.x, worldVector.y);
        }

        [[nodiscard]] sf::Vector2f toScreen(float worldX, float worldY) const noexcept {
            auto [tileX, tileY] = assets()->tileSize();
            return { worldX * tileX, worldY * tileY };
        }

        [[nodiscard]] sf::Vector2f toScreen(int worldX, int worldY) const noexcept {
            return toScreen(sf::Vector2i{worldX, worldY});
        }

        void worldScreenView() noexcept {
            auto cameraPos = toScreen(camera->position().xy());
            window->setView(createFullscreenView(cameraPos, 512, window->getSize()));
        }

        void drawWorld();
        void drawAreas(int level);
        void drawTile(sf::Vector3i position);
        void draw(PlayerMap::SeenActor actor);
        void draw(core::Sound sound);

        void drawHpBar(sf::Vector2f screenPosition, sf::Vector2f origin, double hp, double maxHp, sf::Vector2f maxSize, double colorMod = 1.0);
        void drawSprite(sf::Vector2f screenPosition, sf::Vector2f origin, const sf::Texture& texture, double colorMod = 1.0);

        void drawInWorldRect(sf::IntRect rect,
            sf::Color fillColor, sf::Color outlineColor, float outlineThickness);

        void drawInWorldRect(sf::IntRect rect, sf::Color color) {
            drawInWorldRect(rect, color, sf::Color::Transparent, 0.0f);
        }

        void drawText(sf::Vector2f position, const std::string& text, sf::Color color, int characterSize);
    };
}

#endif