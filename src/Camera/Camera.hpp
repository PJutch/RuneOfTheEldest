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

#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <SFML/Window.hpp>
#include <SFML/System.hpp>

/// Interface for cameras. Determines point of view
class Camera {
public:
    virtual ~Camera() = default;

    /// Position of the Camera
    virtual sf::Vector2f position() const = 0;

    /// Level of the Camera
    virtual int level() const = 0;

    /// Moves Camera to new point. Called on switching Camera
    virtual void moveTo(sf::Vector2f position, int level) = 0;

    /// Resets Camera state
    virtual void reset() = 0;

    /// If true player controls are blocked
    virtual bool shouldStealControl() const {
        return false; 
    }

    /// Notifies camera about sfml event
    virtual void handleEvent(sf::Event event) {}

    /// Updates camera every frame
    virtual void update(sf::Time elapsedTime) {}
};

#endif