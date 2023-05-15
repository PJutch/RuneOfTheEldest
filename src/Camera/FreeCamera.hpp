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

class FreeCamera : public Camera {
public:
    FreeCamera(std::shared_ptr<World> world_, 
               std::shared_ptr<sf::RenderWindow> window);

    sf::View view() const final {
        return view_;
    }

    int level() const final {
        return level_;
    }

    void update(sf::Time elapsedTime) final;
    void handleEvent(sf::Event event) final;
private:
    sf::View view_;
    int level_ = 0;

    std::shared_ptr<World> world;
};

#endif