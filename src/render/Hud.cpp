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

#include "Primitives.hpp"
#include "View.hpp"

#include "core/XpManager.hpp"

namespace render {
    void drawXpBar(sf::RenderTarget& target, const core::XpManager& xpManager) {
        target.setView(sf::View{{0.f, 0.f, 1.f, 1.f}});

        auto xpPercent = static_cast<float>(xpManager.xpPercentUntilNextLvl());
        sf::Vector2f size{xpPercent, 1.f / 128.f};
        sf::FloatRect rect{0, 1.f - size.y, size.x, size.y};

        drawRect(target, rect, {255, 128, 0});
    }
}
