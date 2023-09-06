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

#include "DeathScreen.hpp"

#include "Renderer.hpp"

namespace render {
    void drawDeathScreen(render::Renderer& renderer) {
        renderer.setHudView();
        auto [screenX, screenY] = renderer.viewSize();

        sf::Vector2f youDiedPos(screenX / 2, screenY / 2 - 150.f);
        renderer.drawText(youDiedPos, "You died", sf::Color::Red, 300);

        sf::Vector2f continuePos(screenX / 2, screenY / 2 + 100.f);
        renderer.drawText(continuePos, "Press any key to continue", sf::Color::Red, 50);
    }
}
