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

#include "../util/assert.hpp"

namespace generation {
    Area RandomSizeRoomGenerator::randomRoomIn(const Area& area) {
        const int minSize = 5;

        int width = std::uniform_int_distribution
        { minSize, area.width() }(*randomEngine);
        int height = std::uniform_int_distribution
        { minSize, area.height() }(*randomEngine);

        int left = std::uniform_int_distribution
        { area.left(), area.right() - width }(*randomEngine);
        int top = std::uniform_int_distribution
        { area.top(),  area.bottom() - height }(*randomEngine);

        return Area{ {left, top, width, height} };
    }

    void RandomSizeRoomGenerator::leftPassage(int z, const Area& area, Area& room,
        int y) {
        TROTE_ASSERT(area.top() <= y && y < area.bottom());

        if (room.top() <= y && y < room.bottom() - 1) {
            horizontalPassage(z, area.left(), room.left(), y);
            room.addLeftPassage(y);
        }
        else {
            int turnX = std::uniform_int_distribution
            { area.left(), room.right() - 2 }(*randomEngine);
            horizontalPassage(z, area.left(), turnX + 1, y);
            bendedVerticalPassage(z, room, turnX, y);
        }
    }

    void RandomSizeRoomGenerator::rightPassage(int z, const Area& area, Area& room,
        int y) {
        TROTE_ASSERT(area.top() <= y && y < area.bottom());

        if (room.top() <= y && y < room.bottom() - 1) {
            horizontalPassage(z, room.right(), area.right(), y);
            room.addRightPassage(y);
        }
        else {
            int turnX = std::uniform_int_distribution
            { room.left(), area.right() - 2 }(*randomEngine);
            horizontalPassage(z, turnX, area.right(), y);
            bendedVerticalPassage(z, room, turnX, y);
        }
    }

    void RandomSizeRoomGenerator::topPassage(int z, const Area& area, Area& room,
        int x) {
        TROTE_ASSERT(area.left() <= x && x < area.right());

        if (room.left() <= x && x < room.right() - 1) {
            verticalPassage(z, area.top(), room.top(), x);
            room.addTopPassage(x);
        }
        else {
            int turnY = std::uniform_int_distribution
            { area.top(), room.bottom() - 2 }(*randomEngine);
            verticalPassage(z, area.top(), turnY + 1, x);
            bendedVerticalPassage(z, room, x, turnY);
        }
    }

    void RandomSizeRoomGenerator::bottomPassage(int z, const Area& area, Area& room,
        int x) {
        TROTE_ASSERT(area.left() <= x && x < area.right());

        if (room.left() <= x && x < room.right() - 1) {
            verticalPassage(z, room.bottom(), area.bottom(), x);
            room.addBottomPassage(x);
        }
        else {
            int turnY = std::uniform_int_distribution
            { room.top(), area.bottom() - 2 }(*randomEngine);
            verticalPassage(z, turnY, area.bottom(), x);
            bendedVerticalPassage(z, room, x, turnY);
        }
    }

    void RandomSizeRoomGenerator::bendedHorizontalPassage(int z, Area& room,
        int x, int y) {
        if (room.top() <= y && y < room.bottom() - 1)
            horizontalPassageEnd(z, room, x, y);
        else {
            int turnX = std::uniform_int_distribution
            { room.left(), room.right() - 2 }(*randomEngine);

            if (x < room.left())
                horizontalPassage(z, x, turnX + 1, y);
            else
                horizontalPassage(z, turnX, x, y);

            verticalPassageEnd(z, room, turnX, y);
        }
    }

    void RandomSizeRoomGenerator::bendedVerticalPassage(int z, Area& room, int x, int y) {
        if (room.left() <= x && x < room.right() - 1)
            verticalPassageEnd(z, room, x, y);
        else {
            int turnY = std::uniform_int_distribution
            { room.top(), room.bottom() - 2 }(*randomEngine);

            if (y < room.top())
                verticalPassage(z, y, turnY + 1, x);
            else
                verticalPassage(z, turnY, y, x);

            horizontalPassageEnd(z, room, x, turnY);
        }
    }

    void RandomSizeRoomGenerator::horizontalPassageEnd(int z, Area& room, int x, int y) {
        if (x < room.left()) {
            horizontalPassage(z, x, room.left(), y);
            room.addLeftPassage(y);
        }
        else {
            horizontalPassage(z, room.right(), x, y);
            room.addRightPassage(y);
        }
    }

    void RandomSizeRoomGenerator::verticalPassageEnd(int z, Area& room, int x, int y) {
        if (y < room.top()) {
            verticalPassage(z, y, room.top(), x);
            room.addTopPassage(x);
        }
        else {
            verticalPassage(z, room.bottom(), y, x);
            room.addBottomPassage(x);
        }
    }

    void RandomSizeRoomGenerator::horizontalPassage(int z, int left, int right, int y) {
        for (int x = left; x < right; ++x)
            world->tiles()[{x, y, z}] = (debugTiles_ ? Tile::PASSAGE
                : Tile::EMPTY);
    }

    void RandomSizeRoomGenerator::verticalPassage(int z, int top, int bottom, int x) {
        for (int y = top; y < bottom; ++y)
            world->tiles()[{x, y, z}] = (debugTiles_ ? Tile::PASSAGE
                : Tile::EMPTY);
    }

    void RandomSizeRoomGenerator::operator() (int z, Area area) {
        TROTE_ASSERT(world->tiles().isValidRect(area.bounds()));

        Area room = randomRoomIn(area);

        for (int y : area.leftPassages())
            leftPassage(z, area, room, y);

        for (int y : area.rightPassages())
            rightPassage(z, area, room, y);

        for (int x : area.topPassages())
            topPassage(z, area, room, x);

        for (int x : area.bottomPassages())
            bottomPassage(z, area, room, x);

        generator(z, std::move(room));
    }
}
