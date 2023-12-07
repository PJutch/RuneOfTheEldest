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

#include "randomSizeRoom.hpp"

#include "line.hpp"
#include "basicRoom.hpp"

#include "core/World.hpp"

#include "util/assert.hpp"

namespace generation {
    namespace {
        [[nodiscard]] Area randomRoomIn(const Area& area, util::RandomEngine& randomEngine) {
            const int minSize = 2;

            int width = std::uniform_int_distribution
            {minSize, area.width()}(randomEngine);
            int height = std::uniform_int_distribution
            {minSize, area.height()}(randomEngine);

            int left = std::uniform_int_distribution
            {area.left(), area.right() - width}(randomEngine);
            int top = std::uniform_int_distribution
            {area.top(),  area.bottom() - height}(randomEngine);

            return Area{{left, top, width, height}, area.z()};
        }

        void horizontalPassageEnd(core::World& world, Area& room, int x, int y, bool debugTiles) {
            if (x < room.left()) {
                horizontalLine(world, room.z(), x, room.left(), y, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                room.addLeftPassage(y);
            } else {
                horizontalLine(world, room.z(), room.right(), x, y, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                room.addRightPassage(y);
            }
        }

        void verticalPassageEnd(core::World& world, Area& room, int x, int y, bool debugTiles) {
            if (y < room.top()) {
                verticalLine(world, room.z(), y, room.top(), x, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                room.addTopPassage(x);
            } else {
                verticalLine(world, room.z(), room.bottom(), y, x, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                room.addBottomPassage(x);
            }
        }

        void bendedHorizontalPassage(core::World& world, Area& room, int x, int y, 
                                     bool debugTiles, util::RandomEngine& randomEngine) {
            if (room.top() <= y && y < room.bottom() - 1)
                horizontalPassageEnd(world, room, x, y, debugTiles);
            else {
                int turnX = std::uniform_int_distribution
                {room.left(), room.right() - 2}(randomEngine);

                if (x < room.left())
                    horizontalLine(world, room.z(), x, turnX + 1, y, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                else
                    horizontalLine(world, room.z(), turnX, x, y, debugTiles ? Tile::PASSAGE : Tile::EMPTY);

                verticalPassageEnd(world, room, turnX, y, debugTiles);
            }
        }

        void bendedVerticalPassage(core::World& world, Area& room, int x, int y, 
                                   bool debugTiles, util::RandomEngine& randomEngine) {
            if (room.left() <= x && x < room.right() - 1)
                verticalPassageEnd(world, room, x, y, debugTiles);
            else {
                int turnY = std::uniform_int_distribution
                {room.top(), room.bottom() - 2}(randomEngine);

                if (y < room.top())
                    verticalLine(world, room.z(), y, turnY + 1, x, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                else
                    verticalLine(world, room.z(), turnY, y, x, debugTiles ? Tile::PASSAGE : Tile::EMPTY);

                horizontalPassageEnd(world, room, x, turnY, debugTiles);
            }
        }

        void leftPassage(core::World& world, const Area& area, Area& room, int y, 
                         bool debugTiles, util::RandomEngine& randomEngine) {
            TROTE_ASSERT(area.top() <= y && y < area.bottom());

            if (room.top() <= y && y < room.bottom() - 1) {
                horizontalLine(world, room.z(), area.left(), room.left(), y, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                room.addLeftPassage(y);
            } else {
                int turnX = std::uniform_int_distribution
                {area.left(), room.right() - 2}(randomEngine);
                horizontalLine(world, room.z(), area.left(), turnX + 1, y, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                bendedVerticalPassage(world, room, turnX, y, debugTiles, randomEngine);
            }
        }

        void rightPassage(core::World& world, const Area& area, Area& room, int y,
                          bool debugTiles, util::RandomEngine& randomEngine) {
            TROTE_ASSERT(area.top() <= y && y < area.bottom());

            if (room.top() <= y && y < room.bottom() - 1) {
                horizontalLine(world, room.z(), room.right(), area.right(), y, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                room.addRightPassage(y);
            } else {
                int turnX = std::uniform_int_distribution
                {room.left(), area.right() - 2}(randomEngine);
                horizontalLine(world, room.z(), turnX, area.right(), y, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                bendedVerticalPassage(world, room, turnX, y, debugTiles, randomEngine);
            }
        }

        void topPassage(core::World& world, const Area& area, Area& room, int x,
                        bool debugTiles, util::RandomEngine& randomEngine) {
            TROTE_ASSERT(area.left() <= x && x < area.right());

            if (room.left() <= x && x < room.right() - 1) {
                verticalLine(world, room.z(), area.top(), room.top(), x, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                room.addTopPassage(x);
            } else {
                int turnY = std::uniform_int_distribution
                {area.top(), room.bottom() - 2}(randomEngine);
                verticalLine(world, room.z(), area.top(), turnY + 1, x, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                bendedHorizontalPassage(world, room, x, turnY, debugTiles, randomEngine);
            }
        }

        void bottomPassage(core::World& world, const Area& area, Area& room, int x,
                           bool debugTiles, util::RandomEngine& randomEngine) {
            TROTE_ASSERT(area.left() <= x && x < area.right());

            if (room.left() <= x && x < room.right() - 1) {
                verticalLine(world, room.z(), room.bottom(), area.bottom(), x, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                room.addBottomPassage(x);
            } else {
                int turnY = std::uniform_int_distribution
                {room.top(), area.bottom() - 2}(randomEngine);
                verticalLine(world, room.z(), turnY, area.bottom(), x, debugTiles ? Tile::PASSAGE : Tile::EMPTY);
                bendedHorizontalPassage(world, room, x, turnY, debugTiles, randomEngine);
            }
        }
    }

    void randomSizeRoom(core::World& world, Area area, util::RandomEngine& randomEngine, bool debugTiles) {
        TROTE_ASSERT(world.tiles().isValidRect(area.bounds()));

        Area room = randomRoomIn(area, randomEngine);

        for (int y : area.leftPassages())
            leftPassage(world, area, room, y, debugTiles, randomEngine);

        for (int y : area.rightPassages())
            rightPassage(world,  area, room, y, debugTiles, randomEngine);

        for (int x : area.topPassages())
            topPassage(world, area, room, x, debugTiles, randomEngine);

        for (int x : area.bottomPassages())
            bottomPassage(world, area, room, x, debugTiles, randomEngine);

        randomRoom(world, std::move(room), randomEngine, debugTiles);
    }
}
