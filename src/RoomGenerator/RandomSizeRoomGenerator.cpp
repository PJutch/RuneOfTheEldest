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

#include "../debug.hpp"

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

void RandomSizeRoomGenerator::leftPassage(const Area& area, Area& room, 
                                          int y) {
    if (room.top() <= y && y < room.bottom() - 1) {
        horizontalPassage(area.left(), room.left(), y);
        room.addLeftPassage(y);
    } else {
        int turnX = std::uniform_int_distribution
            {area.left(), room.right() - 2}(*randomEngine);
        horizontalPassage(area.left(), turnX + 1, y);
        bendedVerticalPassage(room, turnX, y);
    }
}

void RandomSizeRoomGenerator::rightPassage(const Area& area, Area& room, 
                                           int y) {
    if (room.top() <= y && y < room.bottom() - 1) {
        horizontalPassage(room.right(), area.right(), y);
        room.addRightPassage(y);
    } else {
        int turnX = std::uniform_int_distribution
            {room.left(), area.right() - 2}(*randomEngine);
        horizontalPassage(turnX, area.right(), y);
        bendedVerticalPassage(room, turnX, y);
    }
}

void RandomSizeRoomGenerator::topPassage(const Area& area, Area& room, 
                                         int x) {
    if (room.left() <= x && x < room.right() - 1) {
        verticalPassage(area.top(), room.top(), x);
        room.addTopPassage(x);
    } else {
        int turnY = std::uniform_int_distribution
            {area.top(), room.bottom() - 2}(*randomEngine);
        verticalPassage(area.top(), turnY + 1, x);
        bendedVerticalPassage(room, x, turnY);
    }
}

void RandomSizeRoomGenerator::bottomPassage(const Area& area, Area& room, 
                                            int x) {                                           
    if (room.left() <= x && x < room.right() - 1) {
        verticalPassage(room.bottom(), area.bottom(), x);
        room.addBottomPassage(x);
    } else {
        int turnY = std::uniform_int_distribution
            {room.top(), area.bottom() - 2}(*randomEngine);
        verticalPassage(turnY, area.bottom(), x);
        bendedVerticalPassage(room, x, turnY);
    }
}

void RandomSizeRoomGenerator::bendedHorizontalPassage(Area& room, 
                                                      int x, int y) {
    if (room.top() <= y && y < room.bottom() - 1)
        horizontalPassageEnd(room, x, y);
    else {
        int turnX = std::uniform_int_distribution
            {room.left(), room.right() - 2}(*randomEngine);

        if (x < room.left())
            horizontalPassage(x, turnX + 1, y);
        else
            horizontalPassage(turnX, x, y);

        verticalPassageEnd(room, turnX, y);
    }
}

void RandomSizeRoomGenerator::bendedVerticalPassage(Area& room, int x, int y) {
    if (room.left() <= x && x < room.right() - 1)
        verticalPassageEnd(room, x, y); 
    else {
        int turnY = std::uniform_int_distribution
            {room.top(), room.bottom() - 2}(*randomEngine);

        if (y < room.top())
            verticalPassage(y, turnY + 1, x);
        else
            verticalPassage(turnY, y, x);

        horizontalPassageEnd(room, x, turnY);
    }
}

void RandomSizeRoomGenerator::horizontalPassageEnd(Area& room, int x, int y) {
    if (x < room.left()) {
        horizontalPassage(x, room.left(), y);
        room.addLeftPassage(x);
    } else {
        horizontalPassage(room.right(), x, y);
        room.addRightPassage(x);
    }
}

void RandomSizeRoomGenerator::verticalPassageEnd(Area& room, int x, int y) {
    if (y < room.top()) {
        verticalPassage(y, room.top(), x);
        room.addTopPassage(x);
    } else {
        verticalPassage(room.bottom(), y, x);
        room.addBottomPassage(x);
    }
}

void RandomSizeRoomGenerator::horizontalPassage(int left, int right, int y) {
    auto level_ = level.lock();
    for (int x = left; x < right; ++ x)
        level_->at(x, y) = (debugTiles ? Level::Tile::PASSAGE
                                       : Level::Tile::EMPTY);
}

void RandomSizeRoomGenerator::verticalPassage(int top, int bottom, int x) {
    auto level_ = level.lock();
    for (int y = top; y < bottom; ++ y)
        level_->at(x, y) = (debugTiles ? Level::Tile::PASSAGE
                                       : Level::Tile::EMPTY);
}

void RandomSizeRoomGenerator::operator() (Area area) {
    level.lock()->addArea(area.bounds());

    Area room = randomRoomIn(area);

    for (int y : area.leftPassages())
        leftPassage(area, room, y);

    for (int y : area.rightPassages())
        rightPassage(area, room, y);
    
    for (int x : area.topPassages())
        topPassage(area, room, x);
    
    for (int x : area.bottomPassages())
        bottomPassage(area, room, x);

    generator(std::move(room));
}
