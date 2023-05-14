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

#ifndef RANDOM_ROOM_GENERATOR_HPP_
#define RANDOM_ROOM_GENERATOR_HPP_

#include "RoomGenerator.hpp"
#include "BasicRoomGenerator.hpp"

#include "../Level.hpp"

#include "../random.hpp"

#include <memory>

class RandomSizeRoomGenerator : public RoomGenerator {
public:
    RandomSizeRoomGenerator(RandomEngine& randomEngine_) : randomEngine{&randomEngine_} {}

    void operator() (std::shared_ptr<Level> level, Area area) final;

    void debugTiles(bool newDebugTiles = true) noexcept final {
        debugTiles_ = newDebugTiles;
        generator.debugTiles(newDebugTiles);
    }
private:
    bool debugTiles_ = false;

    BasicRoomGenerator generator;

    RandomEngine* randomEngine;

    Area randomRoomIn(const Area& area);

    void leftPassage(std::shared_ptr<Level> level, const Area& area, Area& room, int y);
    void topPassage(std::shared_ptr<Level> level, const Area& area, Area& room, int y);
    void rightPassage(std::shared_ptr<Level> level, const Area& area, Area& room, int y);
    void bottomPassage(std::shared_ptr<Level> level, const Area& area, Area& room, int y);

    void bendedHorizontalPassage(std::shared_ptr<Level> level, Area& room, int x, int y);
    void bendedVerticalPassage(std::shared_ptr<Level> level, Area& room, int x, int y);

    void horizontalPassageEnd(std::shared_ptr<Level> level, Area& room, int x, int y);
    void verticalPassageEnd(std::shared_ptr<Level> level, Area& room, int x, int y);

    void horizontalPassage(std::shared_ptr<Level> level, int left, int right, int y);
    void verticalPassage(std::shared_ptr<Level> level, int top, int bottom, int x);
};

#endif