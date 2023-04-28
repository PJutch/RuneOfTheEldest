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

#include "Area.hpp"

std::pair<Area, Area> Area::splitX(int boundary) const noexcept {
    Area leftHalf{{left(), top(), boundary - left(), area.height}};
    Area rightHalf{{boundary, top(), right() - boundary, area.height}};

    leftHalf.leftPassages_ = leftPassages_;
    rightHalf.rightPassages_ = rightPassages_;

    for (int topPassage : topPassages())
        if (topPassage < boundary)
            leftHalf.topPassages_.push_back(topPassage);
        else
            rightHalf.topPassages_.push_back(topPassage);

    for (int bottomPassage : bottomPassages())
        if (bottomPassage < boundary)
            leftHalf.bottomPassages_.push_back(bottomPassage);
        else
            rightHalf.bottomPassages_.push_back(bottomPassage);

    return {std::move(leftHalf), std::move(rightHalf)};
}

std::pair<Area, Area> Area::splitY(int boundary) const noexcept {
    Area topHalf{{left(), top(), area.width, boundary - top()}};
    Area bottomHalf{{left(), boundary, area.width, bottom() - boundary}};

    topHalf.topPassages_ = topPassages_;
    bottomHalf.bottomPassages_ = bottomPassages_;

    for (int leftPassage : leftPassages())
        if (leftPassage < boundary)
            topHalf.leftPassages_.push_back(leftPassage);
        else
            bottomHalf.leftPassages_.push_back(leftPassage);

    for (int rightPassage : rightPassages())
        if (rightPassage < boundary)
            topHalf.rightPassages_.push_back(rightPassage);
        else
            bottomHalf.rightPassages_.push_back(rightPassage);

    return {std::move(topHalf), std::move(bottomHalf)};
}
