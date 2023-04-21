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

    std::pair<Area, Area> splitX(int boundary) const noexcept {
        return {Area{{left(), top(), boundary - left(), area.height}},
                Area{{boundary, top(), right() - boundary, area.height}}};
    }

    std::pair<Area, Area> splitY(int boundary) const noexcept {
        return {Area{{left(), top(), area.width, boundary - top()}},
                Area{{left(), boundary, area.width, bottom() - boundary}}};
    }
private:
    sf::IntRect area;
};

#endif