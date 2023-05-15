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

#include "RandomSizeRoomGenerator.hpp"

#include "../assert.hpp"

Area RandomSizeRoomGenerator::randomRoomIn(const Area& area) {
    const int minSize = 5;

    int width  = std::uniform_int_distribution
        {minSize, area.width()}(*randomEngine);
    int height = std::uniform_int_distribution
        {minSize, area.height()}(*randomEngine);
    
    int left = std::uniform_int_distribution
        {area.left(), area.right()  -  width}(*randomEngine);
    int top  = std::uniform_int_distribution
        {area.top(),  area.bottom() - height}(*randomEngine);
    
    return Area{{left, top, width, height}};
}

void RandomSizeRoomGenerator::leftPassage(Level& level, const Area& area, Area& room,
                                          int y) {
    TROTE_ASSERT(area.top() <= y && y < area.bottom());

    if (room.top() <= y && y < room.bottom() - 1) {
        horizontalPassage(level, area.left(), room.left(), y);
        room.addLeftPassage(y);
    } else {
        int turnX = std::uniform_int_distribution
            {area.left(), room.right() - 2}(*randomEngine);
        horizontalPassage(level, area.left(), turnX + 1, y);
        bendedVerticalPassage(level, room, turnX, y);
    }
}

void RandomSizeRoomGenerator::rightPassage(Level& level, const Area& area, Area& room,
                                           int y) {
    TROTE_ASSERT(area.top() <= y && y < area.bottom());

    if (room.top() <= y && y < room.bottom() - 1) {
        horizontalPassage(level, room.right(), area.right(), y);
        room.addRightPassage(y);
    } else {
        int turnX = std::uniform_int_distribution
            {room.left(), area.right() - 2}(*randomEngine);
        horizontalPassage(level, turnX, area.right(), y);
        bendedVerticalPassage(level, room, turnX, y);
    }
}

void RandomSizeRoomGenerator::topPassage(Level& level, const Area& area, Area& room,
                                         int x) {
    TROTE_ASSERT(area.left() <= x && x < area.right());

    if (room.left() <= x && x < room.right() - 1) {
        verticalPassage(level, area.top(), room.top(), x);
        room.addTopPassage(x);
    } else {
        int turnY = std::uniform_int_distribution
            {area.top(), room.bottom() - 2}(*randomEngine);
        verticalPassage(level, area.top(), turnY + 1, x);
        bendedVerticalPassage(level, room, x, turnY);
    }
}

void RandomSizeRoomGenerator::bottomPassage(Level& level, const Area& area, Area& room,
                                            int x) {
    TROTE_ASSERT(area.left() <= x && x < area.right());
                                               
    if (room.left() <= x && x < room.right() - 1) {
        verticalPassage(level, room.bottom(), area.bottom(), x);
        room.addBottomPassage(x);
    } else {
        int turnY = std::uniform_int_distribution
            {room.top(), area.bottom() - 2}(*randomEngine);
        verticalPassage(level, turnY, area.bottom(), x);
        bendedVerticalPassage(level, room, x, turnY);
    }
}

void RandomSizeRoomGenerator::bendedHorizontalPassage(Level& level, Area& room,
                                                      int x, int y) {
    if (room.top() <= y && y < room.bottom() - 1)
        horizontalPassageEnd(level, room, x, y);
    else {
        int turnX = std::uniform_int_distribution
            {room.left(), room.right() - 2}(*randomEngine);

        if (x < room.left())
            horizontalPassage(level, x, turnX + 1, y);
        else
            horizontalPassage(level, turnX, x, y);

        verticalPassageEnd(level, room, turnX, y);
    }
}

void RandomSizeRoomGenerator::bendedVerticalPassage(Level& level, Area& room, int x, int y) {
    if (room.left() <= x && x < room.right() - 1)
        verticalPassageEnd(level, room, x, y);
    else {
        int turnY = std::uniform_int_distribution
            {room.top(), room.bottom() - 2}(*randomEngine);

        if (y < room.top())
            verticalPassage(level, y, turnY + 1, x);
        else
            verticalPassage(level, turnY, y, x);

        horizontalPassageEnd(level, room, x, turnY);
    }
}

void RandomSizeRoomGenerator::horizontalPassageEnd(Level& level, Area& room, int x, int y) {
    if (x < room.left()) {
        horizontalPassage(level, x, room.left(), y);
        room.addLeftPassage(y);
    } else {
        horizontalPassage(level, room.right(), x, y);
        room.addRightPassage(y);
    }
}

void RandomSizeRoomGenerator::verticalPassageEnd(Level& level, Area& room, int x, int y) {
    if (y < room.top()) {
        verticalPassage(level, y, room.top(), x);
        room.addTopPassage(x);
    } else {
        verticalPassage(level, room.bottom(), y, x);
        room.addBottomPassage(x);
    }
}

void RandomSizeRoomGenerator::horizontalPassage(Level& level, int left, int right, int y) {
    for (int x = left; x < right; ++ x)
        level.at(x, y) = (debugTiles_ ? Level::Tile::PASSAGE
                                       : Level::Tile::EMPTY);
}

void RandomSizeRoomGenerator::verticalPassage(Level& level, int top, int bottom, int x) {
    for (int y = top; y < bottom; ++ y)
        level.at(x, y) = (debugTiles_ ? Level::Tile::PASSAGE
                                       : Level::Tile::EMPTY);
}

void RandomSizeRoomGenerator::operator() (Level& level, Area area) {
    TROTE_ASSERT(level.isValidRect(area.bounds()));

    Area room = randomRoomIn(area);

    for (int y : area.leftPassages())
        leftPassage(level, area, room, y);

    for (int y : area.rightPassages())
        rightPassage(level, area, room, y);
    
    for (int x : area.topPassages())
        topPassage(level, area, room, x);
    
    for (int x : area.bottomPassages())
        bottomPassage(level, area, room, x);

    generator(level, std::move(room));
}
