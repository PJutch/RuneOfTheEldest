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
        camera_{std::move(camera)}, assets_{std::move(assets_)}, playerMap_{std::move(newPlayerMap)},
        world{std::move(world_)}, xpManager{std::move(xpManager_)},
        raycaster_{std::move(raycaster_)}, window{std::move(window_)} {}

    void draw(render::Renderer& renderer, const core::World& world) {
        renderer.setWorldScreenView();

        for (int x = 0; x < world.tiles().shape().x; ++x)
            for (int y = 0; y < world.tiles().shape().y; ++y)
                drawTile(renderer, world, {x, y, renderer.camera().position().level});

        if (renderer.shouldRenderAreas())
            drawAreas(renderer, world, renderer.camera().position().level);

        for (const auto& actor : renderer.playerMap().seenActors())
            draw(renderer, world, actor);

        for (core::Sound sound : renderer.playerMap().recentSounds())
            draw(renderer, world, sound);
    }

    void drawTile(render::Renderer& renderer, const core::World& world, sf::Vector3i position) {
        const sf::Texture& texture = renderer.assetsRef().tileTexture(world.tiles()[position]);
        sf::Vector2f screenPos = renderer.toScreen(util::getXY(position));

        switch (renderer.playerMap().tileState(position)) {
        case PlayerMap::TileState::VISIBLE: renderer.drawSprite(screenPos, {0, 0}, texture); break;
        case PlayerMap::TileState::MEMORIZED: renderer.drawSprite(screenPos, {0, 0}, texture, 0.5); break;
        case PlayerMap::TileState::UNSEEN: break;
        }
    }

    void drawAreas(render::Renderer& renderer, const core::World& world, int z) {
        for (sf::IntRect area : world.areas(z))
            renderer.drawInWorldRect(area, sf::Color::Transparent, sf::Color::Green, 1.0);
    }

    void draw(render::Renderer& renderer, const core::World& world, PlayerMap::SeenActor actor) {
        bool seen = renderer.playerMap().seeEverything() 
                 || renderer.raycaster().canSee(world.player().position(), actor.position);
        double colorMod = seen ? 1.0 : 0.5;

        if (actor.position.z != renderer.camera().position().level)
            return;

        sf::Vector2f spriteSize = util::geometry_cast<float>(actor.texture->getSize());
        sf::Vector2f tileSize = util::geometry_cast<float>(renderer.assetsRef().tileSize());
        sf::Vector2f aiStateIconSize = util::geometry_cast<float>(renderer.assetsRef().aiStateIcon(actor.aiState).getSize());
        sf::Vector2f maxHpBarSize{spriteSize.x, 2.f};

        sf::Vector2f topLeft = renderer.toScreen(util::getXY(actor.position)) + util::bottomMiddle(tileSize) - util::bottomMiddle(spriteSize);

        renderer.drawSprite(topLeft, {0, 0}, *actor.texture, colorMod);
        drawHpBar(renderer, world, topLeft + util::bottomLeft(spriteSize), util::bottomLeft(maxHpBarSize),
            actor.hp, actor.maxHp, maxHpBarSize, colorMod);
        renderer.drawSprite(topLeft + util::topRight(spriteSize), util::topRight(aiStateIconSize),
            renderer.assetsRef().aiStateIcon(actor.aiState), colorMod);
    }

    void draw(render::Renderer& renderer, const core::World& world, core::Sound sound) {
        if (renderer.raycaster().canSee(sound.position, world.player().position()))
            return;

        if (sound.volume(world.player().position()) < 0.01)
            return;

        renderer.drawSprite(renderer.toScreen(util::getXY(sound.position)), {0, 0}, renderer.assetsRef().soundIcon(sound.type, sound.isSourceOnPlayerSide));
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

    void drawHpBar(render::Renderer& renderer, const core::World& world, sf::Vector2f screenPosition, sf::Vector2f origin,
                   double hp, double maxHp, sf::Vector2f maxSize, double colorMod) {
        double hpFraction = hp / maxHp;
        sf::Color color{static_cast<sf::Uint8>((1 - hpFraction) * colorMod * 255),
                         static_cast<sf::Uint8>(hpFraction * colorMod * 255), 0};

        sf::Vector2f size{static_cast<float>(hpFraction * maxSize.x), maxSize.y};
        sf::FloatRect rect{screenPosition - origin, size};

        renderer.drawRect(rect, color);
    }

    void draw(render::Renderer& renderer, const core::World& world, const core::XpManager& xpManager) {
        renderer.clear();
        draw(renderer, world);
        drawHud(renderer, world, xpManager);
        renderer.display();
    }

    void drawDeathScreen(render::Renderer& renderer) {
        renderer.clear();
        renderer.setHudView();

        auto [screenX, screenY] = renderer.viewSize();

        sf::Vector2f youDiedPos(screenX / 2, screenY / 2 - 150.f);
        renderer.drawText(youDiedPos, "You died", sf::Color::Red, 300);

        sf::Vector2f continuePos(screenX / 2, screenY / 2 + 100.f);
        renderer.drawText(continuePos, "Press any key to continue", sf::Color::Red, 50);

        renderer.display();
    }

    void Renderer::drawText(sf::Vector2f position, const std::string& string, sf::Color color, int characterSize) {
        sf::Text text;
        text.setString(string);
        text.setFont(assetsRef().font());
        text.setFillColor(color);
        text.setCharacterSize(characterSize);

        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.width / 2, textBounds.height / 2);
        text.setPosition(position);

        window->draw(text);
    }

    void Renderer::update(sf::Time elapsedTime) {
        camera_->update(elapsedTime);
        playerMap_->update();
    }

    void drawXpBar(render::Renderer& renderer, const core::World& world, const core::XpManager& xpManager) {
        double xpPercent = xpManager.xpPercentUntilNextLvl();

        sf::Vector2f size{static_cast<float>(xpPercent * renderer.viewSize().x), static_cast<float>(renderer.viewSize().y / 128)};
        sf::FloatRect rect{0, renderer.viewSize().y - size.y, size.x, size.y};

        renderer.drawRect(rect, {255, 128, 0});
    }
}
