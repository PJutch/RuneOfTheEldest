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

#include "DungeonGenerator.hpp"

#include "assert.hpp"
#include "geometry.hpp"

DungeonGenerator::DungeonGenerator(std::unique_ptr<RoomGenerator> newRoomGenerator,
                                   std::shared_ptr<World> world_,
                                   RandomEngine& randomEngine_) :
        roomGenerator_{std::move(newRoomGenerator)},
        world{std::move(world_)},
        randomEngine{&randomEngine_} {
    roomGenerator().dungeon(world->dungeon());
}

void DungeonGenerator::operator() () {
    for (int level = 0; level < world->dungeon().shape().z; ++level)
        processLevel(level);
}

void DungeonGenerator::processLevel(int z) {
    areas.emplace(shrinkTopLeft(world->dungeon().horizontalBounds(), {1, 1}));
    while (!areas.empty()) {
        Area area = std::move(areas.front());
        areas.pop();

        processArea(z, std::move(area));
    }
}

void DungeonGenerator::processArea(int z, Area area) {
    TROTE_ASSERT(world->dungeon().isValidRect(area.bounds()));
    TROTE_ASSERT(area.width() >= minSize_);
    TROTE_ASSERT(area.height() >= minSize_);

    world->addArea(area.bounds(), z);

    if (!canSplit(area.width()) && !canSplit(area.height())) {
        roomGenerator()(z, area);
        return;
    }

    if (std::uniform_real_distribution{}(*randomEngine) > splitChance_) {
        roomGenerator()(z, area);
        return;
    }
        
    if (!canSplit(area.width())) {
        splitY(area);
        return;
    }

    if (!canSplit(area.height())) {
        splitX(area);
        return;
    }

    if (std::uniform_real_distribution{}(*randomEngine) < 0.5)
        splitX(area);
    else
        splitY(area);       
}

void DungeonGenerator::splitX(Area area) {
    TROTE_ASSERT(canSplit(area.width()));

    int boundary = std::uniform_int_distribution
        {area.left() + minSize_, area.right() - minSize_}(*randomEngine);
    auto [left, right] = area.splitX(boundary);

    int passageY = std::uniform_int_distribution
        {area.top(), area.bottom() - 2}(*randomEngine);
    left.addRightPassage(passageY);
    right.addLeftPassage(passageY);

    areas.push(std::move(left));
    areas.push(std::move(right));
}

void DungeonGenerator::splitY(Area area) {
    TROTE_ASSERT(canSplit(area.height()));

    int boundary = std::uniform_int_distribution
        {area.top() + minSize_, area.bottom() - minSize_}(*randomEngine);
    auto [top, bottom] = area.splitY(boundary);

    int passageX = std::uniform_int_distribution
        {area.left(), area.right() - 2}(*randomEngine);
    top.addBottomPassage(passageX);
    bottom.addTopPassage(passageX);

    areas.push(std::move(top));
    areas.push(std::move(bottom));
}
