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

#include "Hud.hpp"

#include "Renderer.hpp"

#include "core/XpManager.hpp"

namespace render {
    void drawXpBar(Renderer& renderer, const core::XpManager& xpManager) {
        double xpPercent = xpManager.xpPercentUntilNextLvl();

        sf::Vector2f size{static_cast<float>(xpPercent * renderer.viewSize().x), 
                          static_cast<float>(renderer.viewSize().y) / 128};
        sf::FloatRect rect{0, renderer.viewSize().y - size.y, size.x, size.y};

        renderer.drawRect(rect, {255, 128, 0});
    }
}
