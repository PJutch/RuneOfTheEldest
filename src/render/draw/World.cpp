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

#include "World.hpp"

#include "Primitives.hpp"
#include "render/coords.hpp"
#include "render/PlayerMap.hpp"
#include "render/AssetManager.hpp"
#include "render/View.hpp"
#include "render/coords.hpp"

#include "core/World.hpp"
#include "core/Sound.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

namespace render {
    namespace {
        void drawTile(sf::RenderTarget& target, const AssetManager& assets,
                      const core::World& world, const render::PlayerMap& playerMap,
                      sf::Vector3i position) {
            const sf::Texture& texture = assets.tileTexture(world.tiles()[position]);
            sf::Vector2f screenPos = toScreen(util::getXY(position));

            switch (playerMap.tileState(position)) {
            case PlayerMap::TileState::VISIBLE: drawSprite(target, screenPos, {0, 0}, texture); break;
            case PlayerMap::TileState::MEMORIZED: drawSprite(target, screenPos, {0, 0}, texture, 0.5); break;
            case PlayerMap::TileState::UNSEEN: break;
            }
        }

        void drawAreas(sf::RenderTarget& target, const core::World& world, core::Position<float> cameraPos) {
            const bool shouldDraw = false;
            if (!shouldDraw)
                return;

            for (sf::IntRect area : world.areas(cameraPos.z))
                drawInWorldRect(target, area, sf::Color::Transparent, sf::Color::Green, 1.0);
        }

        void draw(sf::RenderTarget& target, const AssetManager& assets,
                  const core::World& world, const render::PlayerMap& playerMap,
                  core::Sound sound) {
            if (playerMap.canSee(sound.position))
                return;

            if (sound.volume(world.player().position()) < 0.01)
                return;

            const auto& icon = assets.soundIcon(sound.type, sound.isSourceOnPlayerSide);
            drawSprite(target, toScreen(util::getXY(sound.position)), {0, 0}, icon);
        }

        void drawHpBar(sf::RenderTarget& target, sf::Vector2f screenPosition, sf::Vector2f origin,
            double hp, double maxHp, sf::Vector2f maxSize, double colorMod) {
            double hpFraction = hp / maxHp;
            sf::Color color{static_cast<sf::Uint8>((1 - hpFraction) * colorMod * 255),
                            static_cast<sf::Uint8>(hpFraction * colorMod * 255), 0};

            sf::Vector2f size{static_cast<float>(hpFraction * maxSize.x), maxSize.y};
            sf::FloatRect rect{screenPosition - origin, size};

            drawRect(target, rect, color);
        }

        void drawManaBar(sf::RenderTarget& target, sf::Vector2f screenPosition, sf::Vector2f origin,
                         double mana, double maxMana, sf::Vector2f maxSize, double colorMod) {
            if (maxMana == 0)
                return;

            double manaFraction = mana / maxMana;
            sf::Color color{static_cast<sf::Uint8>((1 - manaFraction) * colorMod * 255),
                            static_cast<sf::Uint8>(manaFraction * colorMod * 255), 
                            static_cast<sf::Uint8>(manaFraction * colorMod * 255)};

            sf::Vector2f size{static_cast<float>(manaFraction * maxSize.x), maxSize.y};
            sf::FloatRect rect{screenPosition - origin, size};

            drawRect(target, rect, color);
        }

        void drawCurrentTile(sf::RenderTarget& target, core::Position<float> cameraPos) {
            auto tile = mouseTile(sf::Mouse::getPosition(), cameraPos, target).xy();
            drawInWorldRect(target, sf::IntRect{tile, {1, 1}}, sf::Color::Transparent, sf::Color::Red, 1.0);
        }

        void draw(sf::RenderTarget& target, const AssetManager& assets,
                  const render::PlayerMap& playerMap, core::Position<float> cameraPos,
                  PlayerMap::SeenActor actor) {
            if (actor.position.z != cameraPos.z)
                return;

            double colorMod = playerMap.canSee(actor.position) ? 1.0 : 0.5;

            sf::Vector2f spriteSize = util::geometry_cast<float>(actor.texture->getSize());
            sf::Vector2f aiStateIconSize = util::geometry_cast<float>(assets.aiStateIcon(actor.aiState).getSize());
            sf::Vector2f maxHpBarSize{spriteSize.x, 2.f};

            sf::Vector2f topLeft = toScreen(util::getXY(actor.position))
                + util::bottomMiddle(util::geometry_cast<float>(render::tileSize))
                - util::bottomMiddle(spriteSize);

            drawSprite(target, topLeft, {0, 0}, *actor.texture, colorMod);
            drawHpBar(target, topLeft + util::bottomLeft(spriteSize), util::bottomLeft(maxHpBarSize),
                actor.hp, actor.maxHp, maxHpBarSize, colorMod);
            drawManaBar(target, topLeft + util::bottomLeft(spriteSize), {0, -0.5f},
                actor.mana, actor.maxMana, maxHpBarSize, colorMod);
            drawSprite(target, topLeft + util::topRight(spriteSize), util::topRight(aiStateIconSize),
                assets.aiStateIcon(actor.aiState), colorMod);
        }
    }

    void draw(sf::RenderTarget& target, const AssetManager& assets,
              const core::World& world, const render::PlayerMap& playerMap, core::Position<float> cameraPos) {
        target.setView(createFullscreenView(toScreen(cameraPos.xy()), 512.f, target.getSize()));

        for (int x = 0; x < world.tiles().shape().x; ++x)
            for (int y = 0; y < world.tiles().shape().y; ++y)
                drawTile(target, assets, world, playerMap, {x, y, cameraPos.z});

        drawAreas(target, world, cameraPos);

        for (const auto& actor : playerMap.seenActors())
            draw(target, assets, playerMap, cameraPos, actor);

        for (const core::Sound& sound : playerMap.recentSounds())
            draw(target, assets, world, playerMap, sound);

        drawCurrentTile(target, cameraPos);
    }
}
