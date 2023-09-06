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
            std::shared_ptr<core::XpManager> xpManager,
            std::shared_ptr<util::Raycaster> raycaster,
            std::shared_ptr<AssetManager> assets);

        /// If true bsp areas created by dungeon generation are rendered
        void renderAreas(bool newRenderAreas = true) noexcept {
            renderAreas_ = newRenderAreas;
        }

        /// If true bsp areas created by dungeon generation are rendered
        bool shouldRenderAreas() noexcept {
            return renderAreas_;
        }

        PlayerMap& playerMap() noexcept {
            return *playerMap_;
        }

        const PlayerMap& playerMap() const noexcept {
            return *playerMap_;
        }

        const Camera& camera() const noexcept {
            return *camera_;
        }

        Camera& camera() noexcept {
            return *camera_;
        }

        /// Updates Renderer components such as Camera
        void update(sf::Time elapsedTime);

        /// Notifies Renderer components such as PlayerMap about world generation
        void onGenerate() {
            playerMap_->onGenerate();
            camera_->reset();
        }

        void clear() {
            window->clear(sf::Color::Black);
        }

        void display() {
            window->display();
        }

        void setWorldScreenView() noexcept {
            window->setView(worldScreenView());
        }

        void setHudView() noexcept {
            window->setView(hudView());
        }

        sf::Vector2f viewSize() const noexcept {
            return window->getView().getSize();
        }

        sf::Vector2f pixelToCoords(sf::Vector2i pixel) const noexcept {
            return window->mapPixelToCoords(pixel);
        }

        std::shared_ptr<AssetManager> assets() const noexcept {
            return assets_;
        }

        const AssetManager& assetsRef() const noexcept {
            return *assets_;
        }

        util::Raycaster& raycaster() const noexcept {
            return *raycaster_;
        }

        [[nodiscard]] sf::Vector2f toScreen(sf::Vector2i worldVector) const noexcept {
            return toScreen(util::geometry_cast<float>(worldVector));
        }

        [[nodiscard]] sf::Vector2f toScreen(sf::Vector2f worldVector) const noexcept {
            return toScreen(worldVector.x, worldVector.y);
        }

        [[nodiscard]] sf::Vector2f toScreen(float worldX, float worldY) const noexcept {
            auto [tileX, tileY] = assets()->tileSize();
            return {worldX * tileX, worldY * tileY};
        }

        [[nodiscard]] sf::Vector2f toScreen(int worldX, int worldY) const noexcept {
            return toScreen(sf::Vector2i{worldX, worldY});
        }

        void drawRect(sf::FloatRect rect,
            sf::Color fillColor, sf::Color outlineColor, float outlineThickness);

        void drawRect(sf::FloatRect rect, sf::Color color) {
            drawRect(rect, color, sf::Color::Transparent, 0.0f);
        }

        void drawInWorldRect(sf::IntRect rect,
            sf::Color fillColor, sf::Color outlineColor, float outlineThickness) {
            drawRect({toScreen(rect.top, rect.left), toScreen(rect.width, rect.height)},
                fillColor, outlineColor, outlineThickness);
        }

        void drawInWorldRect(sf::IntRect rect, sf::Color color) {
            drawInWorldRect(rect, color, sf::Color::Transparent, 0.0f);
        }

        void drawSprite(sf::Vector2f screenPosition, sf::Vector2f origin, const sf::Texture& texture, double colorMod = 1.0, float scale = 1.0);
    
        void drawText(sf::Vector2f position, const std::string& text, sf::Color color, int characterSize);
    private:
        bool renderAreas_ = false;

        std::shared_ptr<Camera> camera_;
        std::shared_ptr<AssetManager> assets_;
        std::shared_ptr<PlayerMap> playerMap_;

        std::shared_ptr<core::World> world;
        std::shared_ptr<core::XpManager> xpManager;

        std::shared_ptr<util::Raycaster> raycaster_;

        std::shared_ptr<sf::RenderWindow> window;

        sf::View worldScreenView() noexcept {
            auto cameraPos = toScreen(camera().position().xy());
            return createFullscreenView(cameraPos, 512, window->getSize());
        }

        sf::View hudView() noexcept {
            return createFullscreenView(1024.f, window->getSize());
        }
    };
}

#endif