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
#include "coords.hpp"

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
        Renderer(std::shared_ptr<sf::RenderWindow> window,
                 std::shared_ptr<AssetManager> assets);

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

        sf::RenderTarget& target() noexcept {
            return *window;
        }
    
        void drawText(sf::Vector2f position, std::string_view text, sf::Color color, int characterSize);
    private:
        std::shared_ptr<AssetManager> assets_;

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