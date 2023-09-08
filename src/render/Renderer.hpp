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
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <string_view>

namespace render {
    /// Renders game world
    class Renderer {
    public:
        /// Creates renderer and loads textures
        Renderer(std::shared_ptr<Camera> camera,
            std::shared_ptr<PlayerMap> playerMap,
            std::shared_ptr<sf::RenderWindow> window,
            std::shared_ptr<AssetManager> assets);

        PlayerMap& playerMap() noexcept {
            return *playerMap_;
        }

        const PlayerMap& playerMap() const noexcept {
            return *playerMap_;
        }

        /// Notifies Renderer components such as PlayerMap about world generation
        void onGenerate() {
            playerMap_->onGenerate();
        }

        void clear() {
            window->clear(sf::Color::Black);
        }

        void display() {
            window->display();
        }

        void setWorldScreenView(sf::Vector2f cameraPos) noexcept {
            window->setView(worldScreenView(cameraPos));
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

        const AssetManager& assets() const noexcept {
            return *assets_;
        }

        [[nodiscard]] sf::Vector2f toScreen(sf::Vector2i worldVector) const noexcept {
            return toScreen(util::geometry_cast<float>(worldVector));
        }

        [[nodiscard]] sf::Vector2f toScreen(sf::Vector2f worldVector) const noexcept {
            return toScreen(worldVector.x, worldVector.y);
        }

        [[nodiscard]] sf::Vector2f toScreen(float worldX, float worldY) const noexcept {
            auto [tileX, tileY] = assets().tileSize();
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
    
        void drawText(sf::Vector2f position, std::string_view text, sf::Color color, int characterSize);
    private:
        std::shared_ptr<AssetManager> assets_;
        std::shared_ptr<PlayerMap> playerMap_;

        std::shared_ptr<sf::RenderWindow> window;

        sf::View worldScreenView(sf::Vector2f cameraPos) noexcept {
            return createFullscreenView(toScreen(cameraPos), 512, window->getSize());
        }

        sf::View hudView() noexcept {
            return createFullscreenView(1024.f, window->getSize());
        }
    };
}

#endif