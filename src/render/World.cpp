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

#include "Renderer.hpp"
#include "coords.hpp"

#include "core/World.hpp"

namespace render {
    void draw(Renderer& renderer, const core::World& world, const render::PlayerMap& playerMap, int z) {
        for (int x = 0; x < world.tiles().shape().x; ++x)
            for (int y = 0; y < world.tiles().shape().y; ++y)
                drawTile(renderer, world, playerMap, {x, y, z});

        drawAreas(renderer, world, z);

        for (const auto& actor : playerMap.seenActors())
            draw(renderer, world, playerMap, z, actor);

        for (core::Sound sound : playerMap.recentSounds())
            draw(renderer, world, playerMap, sound);
    }

    void drawTile(render::Renderer& renderer, const core::World& world, const render::PlayerMap& playerMap,
                  sf::Vector3i position) {
        const sf::Texture& texture = renderer.assets().tileTexture(world.tiles()[position]);
        sf::Vector2f screenPos = toScreen(util::getXY(position));

        switch (playerMap.tileState(position)) {
        case PlayerMap::TileState::VISIBLE: renderer.drawSprite(screenPos, {0, 0}, texture); break;
        case PlayerMap::TileState::MEMORIZED: renderer.drawSprite(screenPos, {0, 0}, texture, 0.5); break;
        case PlayerMap::TileState::UNSEEN: break;
        }
    }

    void drawAreas(render::Renderer& renderer, const core::World& world, int z) {
        const bool shouldDraw = false;
        if (!shouldDraw)
            return;

        for (sf::IntRect area : world.areas(z))
            renderer.drawInWorldRect(area, sf::Color::Transparent, sf::Color::Green, 1.0);
    }

    void draw(render::Renderer& renderer, const core::World& world, const render::PlayerMap& playerMap, int z,
              PlayerMap::SeenActor actor) {
        if (actor.position.z != z)
            return;

        double colorMod = playerMap.canSee(actor.position) ? 1.0 : 0.5;

        sf::Vector2f spriteSize = util::geometry_cast<float>(actor.texture->getSize());
        sf::Vector2f tileSize = util::geometry_cast<float>(renderer.assets().tileSize());
        sf::Vector2f aiStateIconSize = util::geometry_cast<float>(renderer.assets().aiStateIcon(actor.aiState).getSize());
        sf::Vector2f maxHpBarSize{spriteSize.x, 2.f};

        sf::Vector2f topLeft = toScreen(util::getXY(actor.position)) 
                             + util::bottomMiddle(tileSize)
                             - util::bottomMiddle(spriteSize);

        renderer.drawSprite(topLeft, {0, 0}, *actor.texture, colorMod);
        drawHpBar(renderer, topLeft + util::bottomLeft(spriteSize), util::bottomLeft(maxHpBarSize),
            actor.hp, actor.maxHp, maxHpBarSize, colorMod);
        renderer.drawSprite(topLeft + util::topRight(spriteSize), util::topRight(aiStateIconSize),
            renderer.assets().aiStateIcon(actor.aiState), colorMod);
    }

    void draw(render::Renderer& renderer, const core::World& world, const render::PlayerMap& playerMap, 
              core::Sound sound) {
        if (playerMap.canSee(sound.position))
            return;

        if (sound.volume(world.player().position()) < 0.01)
            return;

        const auto& icon = renderer.assets().soundIcon(sound.type, sound.isSourceOnPlayerSide);
        renderer.drawSprite(toScreen(util::getXY(sound.position)), {0, 0}, icon);
    }

    void drawHpBar(render::Renderer& renderer, sf::Vector2f screenPosition, sf::Vector2f origin,
                   double hp, double maxHp, sf::Vector2f maxSize, double colorMod) {
        double hpFraction = hp / maxHp;
        sf::Color color{static_cast<sf::Uint8>((1 - hpFraction) * colorMod * 255),
                        static_cast<sf::Uint8>(      hpFraction * colorMod * 255), 0};

        sf::Vector2f size{static_cast<float>(hpFraction * maxSize.x), maxSize.y};
        sf::FloatRect rect{screenPosition - origin, size};

        renderer.drawRect(rect, color);
    }
}
