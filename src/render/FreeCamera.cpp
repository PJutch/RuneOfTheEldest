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

#include "FreeCamera.hpp"

#include "util/Keyboard.hpp"
#include "util/geometry.hpp"

namespace render {
    FreeCamera::FreeCamera(std::shared_ptr<core::World> world_) : world{ std::move(world_) } {
        reset();
    }

    void FreeCamera::update(sf::Time elapsedTime) {
        const float cameraSpeed = 40.f;
        float moved = cameraSpeed * elapsedTime.asSeconds();

        for (int i = 1; i <= 9; ++i)
            if (sf::Keyboard::isKeyPressed(util::numpad(i)))
                move(util::numpadDirections<float>[i - 1] * moved);
    }

    void FreeCamera::handleEvent(sf::Event event) {
        if (util::wasKeyPressed(event, sf::Keyboard::Comma) && event.key.shift) {
            if (position().level > 0)
                --position_.level;
        }
        else if (util::wasKeyPressed(event, sf::Keyboard::Period) && event.key.shift) {
            if (position().level + 1 < world->tiles().shape().z)
                ++position_.level;
        }
    }
}
