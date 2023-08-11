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

#include "core/World.hpp"

#include "util/raycast.hpp"
#include "util/geometry.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace render {
    Renderer::Renderer(std::shared_ptr<Camera> camera,
        std::shared_ptr<PlayerMap> newPlayerMap,
        std::shared_ptr<sf::RenderWindow> window_,
        std::shared_ptr<core::World> world_,
        std::shared_ptr<core::XpManager> xpManager_,
        std::shared_ptr<util::Raycaster> raycaster_,
        std::shared_ptr<AssetManager> assets_) :
        camera{std::move(camera)}, assets_{std::move(assets_)}, playerMap_{std::move(newPlayerMap)},
        world{std::move(world_)}, xpManager{std::move(xpManager_)},
        raycaster{std::move(raycaster_)}, window{std::move(window_)} {}

    void Renderer::drawWorld() {
        for (int x = 0; x < world->tiles().shape().x; ++x)
            for (int y = 0; y < world->tiles().shape().y; ++y)
                drawTile({x, y, camera->position().level});

        if (renderAreas_)
            drawAreas(camera->position().level);

        for (const auto& actor : playerMap_->seenActors())
            draw(actor);

        for (core::Sound sound : playerMap_->recentSounds())
            draw(sound);
    }

    void Renderer::drawTile(sf::Vector3i position) {
        const sf::Texture& texture = assets()->tileTexture(world->tiles()[position]);
        sf::Vector2f screenPos = toScreen(util::getXY(position));

        switch (playerMap_->tileState(position)) {
        case PlayerMap::TileState::VISIBLE: drawSprite(screenPos, {0, 0}, texture); break;
        case PlayerMap::TileState::MEMORIZED: drawSprite(screenPos, {0, 0}, texture, 0.5); break;
        case PlayerMap::TileState::UNSEEN: break;
        }
    }

    void Renderer::drawAreas(int z) {
        for (sf::IntRect area : world->areas(z))
            drawInWorldRect(area, sf::Color::Transparent, sf::Color::Green, 1.0);
    }

    void Renderer::draw(PlayerMap::SeenActor actor) {
        bool seen = playerMap_->seeEverything() || raycaster->canSee(world->player().position(), actor.position);
        double colorMod = seen ? 1.0 : 0.5;

        if (actor.position.z != camera->position().level)
            return;

        sf::Vector2f spriteSize = util::geometry_cast<float>(actor.texture->getSize());
        sf::Vector2f tileSize = util::geometry_cast<float>(assets()->tileSize());
        sf::Vector2f aiStateIconSize = util::geometry_cast<float>(assets()->aiStateIcon(actor.aiState).getSize());
        sf::Vector2f maxHpBarSize{spriteSize.x, 2.f};

        sf::Vector2f topLeft = toScreen(util::getXY(actor.position)) + util::bottomMiddle(tileSize) - util::bottomMiddle(spriteSize);

        drawSprite(topLeft, {0, 0}, *actor.texture, colorMod);
        drawHpBar(topLeft + util::bottomLeft(spriteSize), util::bottomLeft(maxHpBarSize),
            actor.hp, actor.maxHp, maxHpBarSize, colorMod);
        drawSprite(topLeft + util::topRight(spriteSize), util::topRight(aiStateIconSize),
            assets()->aiStateIcon(actor.aiState), colorMod);
    }

    void Renderer::draw(core::Sound sound) {
        if (raycaster->canSee(sound.position, world->player().position()))
            return;

        if (sound.volume(world->player().position()) < 0.01)
            return;

        drawSprite(toScreen(util::getXY(sound.position)), {0, 0}, assets()->soundIcon(sound.type, sound.isSourceOnPlayerSide));
    }

    void Renderer::drawRect(sf::FloatRect rect, sf::Color fillColor, sf::Color outlineColor, float outlineThickness) {
        sf::RectangleShape rectShape{{rect.width, rect.height}};
        rectShape.setPosition(rect.left, rect.top);

        rectShape.setFillColor(fillColor);
        rectShape.setOutlineColor(outlineColor);
        rectShape.setOutlineThickness(outlineThickness);

        window->draw(rectShape);
    }

    void Renderer::drawSprite(sf::Vector2f screenPosition, sf::Vector2f origin, const sf::Texture& texture, double colorMod, float scale) {
        sf::Sprite sprite;
        sprite.setTexture(texture);

        auto spriteColor = static_cast<sf::Uint8>(colorMod * 255);
        sprite.setColor({spriteColor , spriteColor , spriteColor});

        sprite.setScale(scale, scale);

        sprite.setPosition(screenPosition);
        sprite.setOrigin(origin);

        window->draw(sprite);
    }

    void Renderer::drawHpBar(sf::Vector2f screenPosition, sf::Vector2f origin,
        double hp, double maxHp, sf::Vector2f maxSize, double colorMod) {
        double hpFraction = hp / maxHp;
        sf::Color color{static_cast<sf::Uint8>((1 - hpFraction) * colorMod * 255),
                         static_cast<sf::Uint8>(hpFraction * colorMod * 255), 0};

        sf::Vector2f size{static_cast<float>(hpFraction * maxSize.x), maxSize.y};

        sf::RectangleShape rectShape{size};
        rectShape.setPosition(screenPosition);
        rectShape.setOrigin(origin);

        rectShape.setFillColor(color);

        window->draw(rectShape);
    }

    void Renderer::draw() {
        window->clear(sf::Color::Black);

        worldScreenView();
        drawWorld();

        hudView();
        drawHud();

        window->display();
    }

    void Renderer::drawDeathScreen() {
        window->clear(sf::Color::Black);
        window->setView(hudView());

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
        text.setFillColor(color);
        text.setCharacterSize(characterSize);

        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.width / 2, textBounds.height / 2);
        text.setPosition(position);

        window->draw(text);
    }

    void Renderer::update(sf::Time elapsedTime) {
        camera->update(elapsedTime);
        playerMap_->update();
    }

    void Renderer::drawXpBar() {
        double xpPercent = xpManager->xpPercentUntilNextLvl();

        sf::Vector2f size{static_cast<float>(xpPercent * window->getSize().x), static_cast<float>(window->getSize().y / 128)};

        sf::RectangleShape rectShape{size};
        rectShape.setPosition(0, window->getSize().y - size.y);

        rectShape.setFillColor({255, 128, 0});

        window->draw(rectShape);
    }

    namespace {
        const float skillWidth = 256;
        const float skillHeight = 256;
    }

    void Renderer::drawLevelupScreen() {
        window->setView(hudView());

        auto skills = xpManager->availableSkills();

        sf::Vector2f screenSize = window->getView().getSize();

        float leftBoundary = (screenSize.x - skillWidth * skills.size()) / 2;
        float skillXCenter = leftBoundary + skillWidth / 2;
        for (const core::Skill* skill : skills) {
            drawRect({skillXCenter - skillWidth / 2, 300, skillHeight, 250},
                sf::Color{32, 32, 32}, sf::Color{128, 128, 128}, 4.f);

            const sf::Texture& icon = skill->icon();
            drawSprite({skillXCenter, 3 * screenSize.y / 8}, util::geometry_cast<float>(icon.getSize()) / 2.f,
                       icon, 1.0, 8.0);

            drawText({skillXCenter, screenSize.y / 2}, skill->name(), sf::Color::White, 30);

            skillXCenter += skillWidth;
        }
    }

    void Renderer::handleLevelupScreenEvent(sf::Event event) {
        if (event.type != event.MouseButtonPressed)
            return;

        sf::Vector2f clickPos = window->mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

        auto skills = xpManager->availableSkills();

        sf::Vector2f screenSize = window->getView().getSize();

        float leftBoundary = (screenSize.x - skillWidth * skills.size()) / 2;
        float skillXCenter = leftBoundary + skillWidth / 2;
        for (const core::Skill* skill : skills) {
            sf::FloatRect currentSkillRect{skillXCenter - skillWidth / 2, 300, skillWidth, skillHeight};

            if (currentSkillRect.contains(clickPos)) {
                xpManager->levelUp(skill);
                return;
            }

            skillXCenter += skillWidth;
        }
    }
}
