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

#include "SwitchableCamera.hpp"

void SwitchableCamera::handleEvent(sf::Event event) {
    if (wasKeyPressed(event, sf::Keyboard::V))
        nextCamera();
    else
        currentCamera().handleEvent(event);
}

void SwitchableCamera::nextCamera() noexcept {
    sf::Vector2f oldPosition = currentCamera().position();
    int oldLevel = currentCamera().level();

    ++currentCameraIndex;
    currentCameraIndex %= std::ssize(cameras);

    currentCamera().reset(oldPosition, oldLevel);
}
