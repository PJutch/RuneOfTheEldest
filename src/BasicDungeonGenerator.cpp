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

BasicDungeonGenerator::BasicDungeonGenerator(
        std::unique_ptr<RoomGenerator> roomGenerator_,
        RandomEngine& randomEngine_) :
    roomGenerator{std::move(roomGenerator_)},
    randomEngine{&randomEngine_} {}

void BasicDungeonGenerator::operator() () {
    areas.emplace(startRect_);
    while (!areas.empty()) {
        Area area = std::move(areas.front());
        areas.pop();

        processArea(std::move(area));
    }
}

void BasicDungeonGenerator::processArea(Area area) {
    if (!canSplit(area.width()) && !canSplit(area.height())) {
        (*roomGenerator)(area);
        return;
    }

    if (std::uniform_real_distribution{}(*randomEngine) > splitChance_) {
        (*roomGenerator)(area);
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

void BasicDungeonGenerator::splitX(Area area) {
    int boundary = std::uniform_int_distribution
        {area.left() + minSize_, area.right() - minSize_}(*randomEngine);
    auto [left, right] = area.splitX(boundary);

    int passageY = std::uniform_int_distribution
        {area.top(), area.bottom() - 1}(*randomEngine);
    left.addRightPassage(passageY);
    right.addLeftPassage(passageY);

    areas.push(std::move(left));
    areas.push(std::move(right));
}

void BasicDungeonGenerator::splitY(Area area) {
    int boundary = std::uniform_int_distribution
        {area.top() + minSize_, area.bottom() - minSize_}(*randomEngine);
    auto [top, bottom] = area.splitY(boundary);

    int passageX = std::uniform_int_distribution
        {area.left(), area.right() - 1}(*randomEngine);
    top.addBottomPassage(passageX);
    bottom.addTopPassage(passageX);

    areas.push(std::move(top));
    areas.push(std::move(bottom));
}
