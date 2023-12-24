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

#ifndef STINGIFY_HPP_
#define STINGIFY_HPP_

#include "Array3D.hpp"

#include <algorithm>
#include <string>
#include <string_view>
#include <concepts>

namespace util {
    inline std::string strigifyCharMap(Array3D<char> chars) {
        std::string res;
        res.reserve((chars.shape().x + 1) * (chars.shape().y + 1) * chars.shape().z);

        for (int z = 0; z < chars.shape().z; ++z) {
            for (int y = 0; y < chars.shape().y; ++y) {
                for (int x = 0; x < chars.shape().x; ++x) {
                    res.push_back(chars[{x, y, z}]);
                }
                res.push_back('\n');
            }

            if (z != chars.shape().z - 1) {
                for (int x = 0; x < chars.shape().x; ++x) {
                    res.push_back('-');
                }
                res.push_back('\n');
            }
        }
        return res;
    }

    template <typename T>
    std::string stringifyVector3(sf::Vector3<T> vec) {
        return std::format("{}, {}, {}", vec.x, vec.y, vec.z);
    }
}

#endif