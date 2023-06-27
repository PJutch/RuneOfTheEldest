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

#include "core/Player.hpp"
#include "core/Goblin.hpp"

#include "util/raycast.hpp"

namespace render {
    Renderer::Renderer(std::shared_ptr<Camera> camera,
        std::shared_ptr<PlayerMap> playerMap_,
        std::shared_ptr<sf::RenderWindow> window_,
        std::shared_ptr<core::World> world_,
        std::shared_ptr<core::Player> player_,
        std::shared_ptr<AssetManager> assets_) :
        camera{ std::move(camera) }, assets_{ std::move(assets_) }, playerMap{ std::move(playerMap_) },
        world{ std::move(world_) }, player{ player_ },
        window{ std::move(window_) } {}

    void Renderer::drawWorld() {
        drawLevel(camera->position().level);
        for (const auto& actor : playerMap->seenActors())
            draw(*actor);
    }

    void Renderer::drawLevel(int z) {
        for (int x = 0; x < world->tiles().shape().x; ++x)
            for (int y = 0; y < world->tiles().shape().y; ++y)
                if (playerMap->tileState({ x, y, z }) == PlayerMap::TileState::VISIBLE)
                    drawSprite(sf::Vector2i{ x, y }, assets()->tileTexture(world->tiles()[{x, y, z}]));
                else if (playerMap->tileState({ x, y, z }) == PlayerMap::TileState::MEMORIZED)
                    drawSprite(sf::Vector2i{ x, y }, assets()->tileTexture(world->tiles()[{x, y, z}]), 0.5);

        if (renderAreas_) drawAreas(z);
    }

    void Renderer::drawAreas(int z) {
        for (sf::IntRect area : world->areas(z))
            drawInWorldRect(area, sf::Color::Transparent,
                sf::Color::Green, 1.0);
    }

    void Renderer::draw(const core::Actor::DrawMemento& actor) {
        double colorMod = 1.0;
        if (!util::canSee(player->position(), actor.position(), *world))
            colorMod = 0.5;

        drawSprite(actor.position(), actor.texture(), colorMod);
        drawHpBar(actor.position(), actor.hp(), actor.maxHp(), colorMod);
        drawSprite(actor.position(), assets()->aiStateIcon(actor.aiState()), colorMod);
    }

    void Renderer::drawInWorldRect(sf::IntRect rect,
        sf::Color fillColor, sf::Color outlineColor, float outlineThickness) {
        auto [left, top] = toScreen(rect.left, rect.top);
        auto [width, height] = toScreen(rect.width, rect.height);
        drawRect({ left, top, width, height }, fillColor, outlineColor, outlineThickness);
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

    void Renderer::drawSprite(sf::Vector3i position, const sf::Texture& texture, double colorMod) {
        if (camera->position().level != position.z)
            return;

        drawSprite(util::getXY(position), texture, colorMod);
    }

    void Renderer::drawSprite(sf::Vector2i position, const sf::Texture& texture, double colorMod) {
        sf::Sprite sprite;
        sprite.setTexture(texture);
        sprite.setPosition(toScreen(position));

        sf::Uint8 spriteColor = colorMod * 255;
        sprite.setColor({ spriteColor , spriteColor , spriteColor });

        window->draw(sprite);
    }

    void Renderer::drawHpBar(sf::Vector3i position, double hp, double maxHp, double colorMod) {
        if (camera->position().level != position.z)
            return;

        float hpFraction = static_cast<float>(hp) / maxHp;
        sf::Color color((1 - hpFraction) * colorMod * 255, hpFraction * colorMod * 255, 0);

        float width = hpFraction * assets()->tileSize().x;
        float height = 2.f;

        sf::Vector2f screenPos = toScreen(util::getXY(position));
        float top = screenPos.x;
        float left = screenPos.y + assets()->tileSize().y - height;

        drawRect({ top, left, width, height }, color);
    }

    void Renderer::draw() {
        window->clear(sf::Color::Black);

        worldScreenView();
        drawWorld();

        window->display();
    }

    void Renderer::drawDeathScreen() {
        window->clear(sf::Color::Black);
        window->setView(createFullscreenView(1024.f, window->getSize()));

        auto [screenX, screenY] = window->getView().getSize();

        sf::Vector2f youDiedPos(screenX / 2, screenY / 2 - 150.f);
        drawText(youDiedPos, "You died", sf::Color::Red, 300);

        sf::Vector2f continuePos(screenX / 2, screenY / 2 + 100.f);
        drawText(continuePos, "Press any key to continue", sf::Color::Red, 50);

        window->display();
    }

    void Renderer::drawText(sf::Vector2f position, const std::string& string, sf::Color color, int characterSize) {
        sf::Text text;
        text.setString(string);
        text.setFont(assets()->font());
        text.setColor(color);
        text.setCharacterSize(characterSize);

        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.width / 2, textBounds.height / 2);
        text.setPosition(position);

        window->draw(text);
    }

    void Renderer::update(sf::Time elapsedTime) {
        camera->update(elapsedTime);
        playerMap->update();
    }
}
