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

#ifndef FREE_CAMERA_HPP_
#define FREE_CAMERA_HPP_

#include "Camera.hpp"

#include "../World.hpp"

#include <memory>

/// Camera controlled by player
class FreeCamera : public Camera {
public:
    FreeCamera(std::shared_ptr<Dungeon> dungeon_);

    sf::Vector2f position() const noexcept final {
        return position_;
    }

    int level() const noexcept final {
        return level_;
    }

    /// Moves to new point
    void reset(sf::Vector2f newPosition, int newLevel) noexcept final {
        position_ = newPosition;
        level_ = newLevel;
    }

    /// Prevents player charcater moving together with camera
    bool shouldStealControl() const noexcept final { 
        return true;
    }

    /// Smoothly moves by WSAD
    void update(sf::Time elapsedTime) final;

    /// Changes floor by <>
    void handleEvent(sf::Event event) final;
private:
    sf::Vector2f position_;
    int level_ = 0;

    std::shared_ptr<Dungeon> dungeon;
};

#endif