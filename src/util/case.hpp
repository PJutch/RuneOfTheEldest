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

#ifndef CASE_HPP_
#define CASE_HPP_

#include <string>
#include <cctype>

namespace util {
    inline std::string toLower(std::string s) {
        for (char& c : s) {
            c = static_cast<char>(std::tolower(c));
        }
        return s;
    }

    inline std::string toTitle(std::string s) {
        for (int i = 1; i < std::ssize(s); ++i) {
            s[i] = static_cast<char>(std::tolower(s[i]));
        }
        return s;
    }
}

#endif