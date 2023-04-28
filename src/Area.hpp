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

#ifndef AREA_HPP_
#define AREA_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <utility>
#include <vector>
#include <span>

class Area {
public:
    explicit Area(sf::IntRect area_) : area{area_} {}

    int left() const noexcept {
        return area.left;
    }

    int top() const noexcept {
        return area.top;
    }

    int right() const noexcept {
        return area.left + area.width;
    }

    int bottom() const noexcept {
        return area.top + area.height;
    }

    int width() const noexcept {
        return area.width;
    }

    int height() const noexcept {
        return area.height;
    }

    void addLeftPassage(int x) noexcept {
        leftPassages_.push_back(x);
    }

    void addTopPassage(int x) noexcept {
        topPassages_.push_back(x);
    }

    void addRightPassage(int x) noexcept {
        rightPassages_.push_back(x);
    }

    void addBottomPassage(int x) noexcept {
        bottomPassages_.push_back(x);
    }

    std::span<const int> leftPassages() const noexcept {
        return leftPassages_;
    }

    std::span<const int> topPassages() const noexcept {
        return topPassages_;
    }

    std::span<const int> rightPassages() const noexcept {
        return rightPassages_;
    }

    std::span<const int> bottomPassages() const noexcept {
        return bottomPassages_;
    }

    std::pair<Area, Area> splitX(int boundary) const noexcept;
    std::pair<Area, Area> splitY(int boundary) const noexcept;
private:
    sf::IntRect area;

    std::vector<int> leftPassages_;
    std::vector<int> topPassages_;
    std::vector<int> rightPassages_;
    std::vector<int> bottomPassages_;
};

#endif