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

#include "Player.hpp"
#include "Goblin.hpp"

Renderer::Renderer(std::shared_ptr<Camera> camera,
                   std::shared_ptr<sf::RenderWindow> window_,
                   std::shared_ptr<World> world_,
                   std::unique_ptr<AssetManager> assets_) :
    camera{ std::move(camera) }, assets{ std::move(assets_) },
    world{ std::move(world_) },
    window{ std::move(window_) } {}

void Renderer::drawWorld() {
    draw(world->dungeon()[camera->level()]);

    for (std::shared_ptr<Actor> actor : world->actors())
        actor->draw(*this);
}

void Renderer::draw(const Level& level) {
    for (int x = 0; x < level.shape().x; ++ x)
        for (int y = 0; y < level.shape().y; ++ y)
            drawSprite(sf::Vector2i{ x, y }, assets->tileTexture(level.at(x, y)));
    
    if (renderAreas_) drawAreas(level);
}

void Renderer::drawAreas(const Level& level) {
    for (sf::IntRect area : level.areas())
        drawInWorldRect(area, sf::Color::Transparent, 
                                sf::Color::Green, 1.0);
}

void Renderer::drawInWorldRect(sf::IntRect rect, 
        sf::Color fillColor, sf::Color outlineColor, float outlineThickness) {
    auto [left, top] = toScreen(rect.left, rect.top);
    auto [width, height] = toScreen(rect.width, rect.height);
    drawRect({ left, top, width, height}, fillColor, outlineColor, outlineThickness);
}

void Renderer::drawRect(sf::FloatRect rect,
    sf::Color fillColor, sf::Color outlineColor, float outlineThickness) {
    sf::RectangleShape rectShape{ {rect.width, rect.height} };
    rectShape.setPosition(rect.left, rect.top);

    rectShape.setFillColor(fillColor);
    rectShape.setOutlineColor(outlineColor);
    rectShape.setOutlineThickness(outlineThickness);

    window->draw(rectShape);
}

void Renderer::draw(const Player& player) {
    drawSprite(player.position(), assets->playerTexture());
    drawHpBar(player.position(), player.hp(), player.maxHp());
}

void Renderer::draw(const Goblin& goblin) {
    drawSprite(goblin.position(), assets->goblinTexture());
    drawHpBar(goblin.position(), goblin.hp(), goblin.maxHp());
}

void Renderer::drawSprite(sf::Vector3i position, const sf::Texture& texture) {
    if (camera->level() != position.z)
        return;

    drawSprite(getXY(position), texture);
}

void Renderer::drawSprite(sf::Vector2i position, const sf::Texture& texture) {
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setPosition(toScreen(position));

    window->draw(sprite);
}

void Renderer::drawHpBar(sf::Vector3i position, int hp, int maxHp) {
    if (camera->level() != position.z)
        return;

    float hpFraction = static_cast<float>(hp) / maxHp;
    sf::Color color( (1 - hpFraction) * 255, hpFraction * 255, 0);

    float width = hpFraction * assets->tileSize().x;
    float height = 2.f;

    sf::Vector2f screenPos = toScreen(getXY(position));
    float top = screenPos.x;
    float left = screenPos.y + assets->tileSize().y - height;

    drawRect({ top, left, width, height }, color);
}

void Renderer::draw() {
    window->clear(sf::Color::Black);

    worldScreenView();
    drawWorld();

    window->display();
}
