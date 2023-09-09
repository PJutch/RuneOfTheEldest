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

#include "core/Position.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/System/Vector2.hpp>

namespace render {
    /// Interface for cameras. Determines point of view
    class Camera {
    public:
        virtual ~Camera() = default;

        /// Position of the Camera
        [[nodiscard]] virtual core::Position<float> position() const = 0;

        /// Moves Camera to new point. Called on switching Camera
        virtual void moveTo(core::Position<float> position) = 0;

        /// Resets Camera state
        virtual void reset() = 0;

        /// If true player controls are blocked
        [[nodiscard]] virtual bool shouldStealControl() const {
            return false;
        }

        /// Notifies camera about sfml event
        virtual void handleEvent([[maybe_unused]] sf::Event event) {}

        /// Updates camera every frame
        virtual void update([[maybe_unused]] sf::Time elapsedTime) {}
    };
}

#endif