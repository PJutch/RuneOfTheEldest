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

#ifndef BASIC_ROOM_GENERATOR_HPP_
#define BASIC_ROOM_GENERATOR_HPP_

#include "RoomGenerator.hpp"

#include "../Level.hpp"

#include <memory>

class BasicRoomGenerator : public RoomGenerator {
public:
    BasicRoomGenerator(std::weak_ptr<Level> level_) : 
        level{std::move(level_)} {}

    void operator() (Area area) final;
private:
    std::weak_ptr<Level> level;
};

#endif