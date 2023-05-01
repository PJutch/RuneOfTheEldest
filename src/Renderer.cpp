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

#include "Exception.hpp"

#include "debug.hpp"

class TextureLoadError : public RuntimeError {
    using RuntimeError::RuntimeError;
};

template <typename T>
sf::View createFullscreenView(float height, 
                              sf::Vector2<T> screenSize) noexcept {
    return sf::View{{0, 0, height * screenSize.x / screenSize.y, height}};
}

template <typename T>
sf::View createFullscreenView(sf::Vector2f center, 
        float height, sf::Vector2<T> screenSize) noexcept {
    return sf::View{center, {height * screenSize.x / screenSize.y, height}};
}

Renderer::Renderer(std::shared_ptr<sf::RenderWindow> window_, 
                   std::shared_ptr<Level> level_) :
        window{std::move(window_)}, level{std::move(level_)},
        levelView{createFullscreenView(512, window->getSize())} {
    cameraPosition({0, 0});
    
    if (!tileTexture(Level::Tile::EMPTY).loadFromFile("resources/floor.png"))
        throw TextureLoadError{"Unable to load floor tile texture"};
    
    if (!tileTexture(Level::Tile::WALL).loadFromFile("resources/wall.png"))
        throw TextureLoadError{"Unable to load wall tile texture"};
    
    if (!tileTexture(Level::Tile::UNSEEN).loadFromFile("resources/unseen.png"))
        throw TextureLoadError{"Unable to load unseen tile texture"};
    
    fillTexture(tileTexture(Level::Tile::ROOM), tileSize, 
                sf::Color::Red);
    fillTexture(tileTexture(Level::Tile::ROOM_ENTRANCE), tileSize, 
                sf::Color::Magenta);
    fillTexture(tileTexture(Level::Tile::PASSAGE), tileSize, 
                sf::Color::Blue);
}

void Renderer::drawLevel() {
    window->setView(levelView);

    for (int x = 0; x < level->shape().x; ++ x)
        for (int y = 0; y < level->shape().y; ++ y)
            draw(level->at(x, y), {x, y});
    
    if (renderAreas) drawAreas();
}

void Renderer::drawAreas() {
    for (sf::IntRect area : level->areas())
        drawInWorldRect(area, sf::Color::Transparent, 
                                sf::Color::Green, 1.0);
}

void Renderer::draw(Level::Tile tile, sf::Vector2i position) {
    sf::Sprite tileSprite;
    tileSprite.setTexture(tileTexture(tile));
    tileSprite.setPosition(toScreen(position));

    window->draw(tileSprite);
}

void Renderer::drawInWorldRect(sf::IntRect rect, 
        sf::Color fillColor, sf::Color outlineColor, float outlineThickness) {
    sf::RectangleShape rectShape{toScreen({rect.width, rect.height})};
    rectShape.setPosition(toScreen({rect.left, rect.top}));

    rectShape.setFillColor(fillColor);
    rectShape.setOutlineColor(outlineColor);
    rectShape.setOutlineThickness(outlineThickness);

    window->draw(rectShape);
}

void Renderer::draw() {
    window->clear(sf::Color::Black);
    drawLevel();
    window->display();
}

void Renderer::update(sf::Time elapsedTime) {
    float moved = cameraSpeed * elapsedTime.asSeconds();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        cameraPosition(subY(cameraPosition(), moved)); 
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        cameraPosition(addY(cameraPosition(), moved)); 
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        cameraPosition(subX(cameraPosition(), moved)); 
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        cameraPosition(addX(cameraPosition(), moved)); 
}
