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

FreeCamera::FreeCamera(std::shared_ptr<World> world_) :
    world{ std::move(world_) } {}

void FreeCamera::update(sf::Time elapsedTime) {
    const float cameraSpeed = 40.f;

    float moved = cameraSpeed * elapsedTime.asSeconds();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        position_.y -= moved;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        position_.y += moved;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        position_.x -= moved;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        position_.x += moved;
}

void FreeCamera::handleEvent(sf::Event event) {
    if (event.type == event.KeyPressed) {
        if (event.key.code == sf::Keyboard::PageUp) {
            if (level_ > 0)
                -- level_;
        } else if (event.key.code == sf::Keyboard::PageDown) {
            if (level_ + 1 < world->size())
                ++ level_;
        }
    }
}
