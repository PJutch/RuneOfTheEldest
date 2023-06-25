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

#ifndef PLAYER_LOCKED_CAMERA_HPP_
#define PLAYER_LOCKED_CAMERA_HPP_

#include "Camera.hpp"

#include "../Player.hpp"

#include <memory>

namespace render {
    /// Camera always centered on player
    class PlayerLockedCamera : public Camera {
    public:
        PlayerLockedCamera(std::shared_ptr<Player> player_) :
            player{ std::move(player_) } {}

        /// Uses player position
        [[nodiscard]] Position position() const final {
            return Position{ player->position() };
        }

        /// Can't move
        void moveTo(Position) final {}

        /// Ignore reset
        void reset() final {}
    private:
        std::shared_ptr<Player> player;
    };
}

#endif