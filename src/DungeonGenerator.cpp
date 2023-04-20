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

DungeonGenerator::DungeonGenerator(
    std::function<void(sf::IntRect)> generateRoom_,
    int minSize_, double splitChance_, sf::IntRect startRect,
    RandomEngine& randomEngine_) :
        generateRoom{std::move(generateRoom_)}, 
        minSize{minSize_}, splitChance{splitChance_},
        randomEngine{&randomEngine_} {
    areas.push(startRect);
}

void DungeonGenerator::operator() () {
    while (!areas.empty()) {
        sf::IntRect area = areas.front();
        areas.pop();

        processArea(area);
    }
}

void DungeonGenerator::processArea(sf::IntRect area) {
    if (!canSplit(area.width) && !canSplit(area.height)) {
        generateRoom(area);
        return;
    }

    if (std::uniform_real_distribution{}(*randomEngine) > splitChance) {
        generateRoom(area);
        return;
    }
        
    if (!canSplit(area.width)) {
        splitY(area);
        return;
    }

    if (!canSplit(area.height)) {
        splitX(area);
        return;
    }

    if (std::uniform_real_distribution{}(*randomEngine) < 0.5)
        splitX(area);
    else
        splitY(area);       
}

void DungeonGenerator::splitX(sf::IntRect area) {
    int width1 = std::uniform_int_distribution
        {minSize, area.width - minSize}(*randomEngine);
    areas.emplace(area.left, area.top, width1, area.height);

    int left2 = area.left + width1;
    int width2 = area.width - width1;
    areas.emplace(left2, area.top, width2, area.height);
}

void DungeonGenerator::splitY(sf::IntRect area) {
    int height1 = std::uniform_int_distribution
        {minSize, area.height - minSize}(*randomEngine);
    areas.emplace(area.left, area.top, area.width, height1);

    int top2 = area.top + height1;
    int height2 = area.height - height1;
    areas.emplace(area.left, top2, area.width, height2);
}
