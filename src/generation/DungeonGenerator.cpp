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

#include "randomSizeRoom.hpp"

#include "util/assert.hpp"
#include "util/geometry.hpp"

namespace generation {
    DungeonGenerator::DungeonGenerator(std::shared_ptr <core::World> world_, util::RandomEngine& randomEngine_) :
        world{ std::move(world_) }, randomEngine{ &randomEngine_ } {}

    void DungeonGenerator::operator() () {
        for (int level = 0; level < world->tiles().shape().z; ++level)
            processLevel(level);
    }

    void DungeonGenerator::processLevel(int z) {
        areas.emplace(util::shrinkTopLeft(world->tiles().horizontalBounds(), { 1, 1 }), z);
        while (!areas.empty()) {
            Area area = std::move(areas.front());
            areas.pop();

            processArea(std::move(area));
        }
    }

    void DungeonGenerator::processArea(Area area) {
        TROTE_ASSERT(world->tiles().isValidRect(area.bounds()));
        TROTE_ASSERT(area.width() >= minSize_);
        TROTE_ASSERT(area.height() >= minSize_);

        world->addArea(area.bounds(), area.z());

        if (std::uniform_real_distribution{}(*randomEngine) > splitChance_) {
            generation::randomSizeRoom(*world, area, *randomEngine);
            return;
        }

        if (std::uniform_real_distribution{}(*randomEngine) < 0.5) {
            if (!canSplit(area.width()))
                generation::randomSizeRoom(*world, area, *randomEngine);
            else 
                splitX(area);
        } else {
            if (!canSplit(area.height()))
                generation::randomSizeRoom(*world, area, *randomEngine);
            else
                splitY(area);
        }
    }

    const double more_passage_prob = 0.5;

    void DungeonGenerator::splitX(Area area) {
        TROTE_ASSERT(canSplit(area.width()));

        int boundary = std::uniform_int_distribution
            { area.left() + minSize_, area.right() - minSize_ }(*randomEngine);
        auto [left, right] = area.splitX(boundary);

        do {
            int passageY = std::uniform_int_distribution
            {area.top(), area.bottom() - 2}(*randomEngine);
            left.addRightPassage(passageY);
            right.addLeftPassage(passageY);
        } while (std::bernoulli_distribution{more_passage_prob}(*randomEngine));

        areas.push(std::move(left));
        areas.push(std::move(right));
    }

    void DungeonGenerator::splitY(Area area) {
        TROTE_ASSERT(canSplit(area.height()));

        int boundary = std::uniform_int_distribution
            { area.top() + minSize_, area.bottom() - minSize_ }(*randomEngine);
        auto [top, bottom] = area.splitY(boundary);

        do {
            int passageX = std::uniform_int_distribution
                { area.left(), area.right() - 2 }(*randomEngine);
            top.addBottomPassage(passageX);
            bottom.addTopPassage(passageX);
        } while (std::bernoulli_distribution{more_passage_prob}(*randomEngine));

        areas.push(std::move(top));
        areas.push(std::move(bottom));
    }
}
