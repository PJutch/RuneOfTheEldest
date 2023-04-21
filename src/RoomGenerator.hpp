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

#ifndef ROOM_GENERATOR_HPP_
#define ROOM_GENERATOR_HPP_

#include "Level.hpp"

#include "random.hpp"

#include <SFML/Graphics.hpp>

#include <memory>

class RoomGenerator {
public:
    virtual ~RoomGenerator() = default;

    virtual void operator() (sf::IntRect room) = 0;
};

class BasicRoomGenerator : public RoomGenerator {
public:
    BasicRoomGenerator(std::weak_ptr<Level> level_) : 
        level{std::move(level_)} {}

    void operator() (sf::IntRect room) final;
private:
    std::weak_ptr<Level> level;
};

class RandomRoomGenerator : public RoomGenerator {
public:
    RandomRoomGenerator(std::weak_ptr<Level> level, 
                        RandomEngine& randomEngine_) : 
        generator{std::move(level)}, randomEngine{&randomEngine_} {}

    void operator() (sf::IntRect room) final;
private:
    BasicRoomGenerator generator;
    RandomEngine* randomEngine;
};

#endif