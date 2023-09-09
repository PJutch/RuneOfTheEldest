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

#ifndef SWITCHABLE_CAMERA_HPP_
#define SWITCHABLE_CAMERA_HPP_

#include "Camera.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>

#include <vector>
#include <memory>

namespace render {
    /// Cyclically switching camera
    class SwitchableCamera : public Camera {
    public:
        SwitchableCamera(std::vector<std::unique_ptr<Camera>> cameras_) :
            cameras{ std::move(cameras_) } {}

        /// Uses current camera position
        [[nodiscard]] core::Position<float> position() const final {
            return currentCamera().position();
        }

        /// Moves current camera
        void moveTo(core::Position<float> newPosition) final {
            currentCamera().moveTo(newPosition);
        }

        /// Resets all cameras and selects 1st
        void reset() final {
            for (auto& camera : cameras)
                camera->reset();
            currentCameraIndex = 0;
        }

        /// Steal control if current camera does
        [[nodiscard]] bool shouldStealControl() const final {
            return currentCamera().shouldStealControl();
        }

        /// @brief Switches camera on V
        /// @details If current camera is last switches to first camera.
        ///       \n New camera is reset.
        void handleEvent(sf::Event event) final;

        /// Updates current camera
        void update(sf::Time elapsedTime) final {
            currentCamera().update(elapsedTime);
        }
    private:
        std::vector<std::unique_ptr<Camera>> cameras;
        int currentCameraIndex = 0;

        [[nodiscard]] Camera& currentCamera() noexcept {
            return *cameras[currentCameraIndex];
        }

        [[nodiscard]] const Camera& currentCamera() const noexcept {
            return *cameras[currentCameraIndex];
        }

        void nextCamera() noexcept;
    };
}

#endif