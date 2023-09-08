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

#ifndef RENDER_WORLD
#define RENDER_WORLD

#include "render/fwd.hpp"
#include "PlayerMap.hpp"

#include "core/fwd.hpp"
#include "core/Sound.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

namespace render {
    void draw(Renderer& renderer, const core::World& world, int z);
    void drawAreas(Renderer& renderer, const core::World& world, int level);
    void drawTile(Renderer& renderer, const core::World& world, sf::Vector3i position);
    void draw(Renderer& renderer, const core::World& world, int z, PlayerMap::SeenActor actor);
    void draw(Renderer& renderer, const core::World& world, core::Sound sound);
    void drawHpBar(Renderer& renderer, sf::Vector2f screenPosition, sf::Vector2f origin, 
                   double hp, double maxHp, sf::Vector2f maxSize, double colorMod = 1.0);
}

#endif