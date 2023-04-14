# This file is part of the Rune of the Eldest.
# The Rune of the Eldest - Roguelike about the mage seeking for ancient knowledges
# Copyright (C) 2023  PJutch

# The Rune of the Eldest is free software: you can redistribute it and/or modify it 
# under the terms of the GNU General Public License as published by the Free Software Foundation, 
# either version 3 of the License, or (at your option) any later version.

# The Rune of the Eldest is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.

# You should have received a copy of the GNU General Public License along with the Rune of the Eldest. 
# If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.20)

cmake_policy(SET CMP0048 NEW)
project(TheRuneOfTheEldest VERSION 1.0
        DESCRIPTION "Roguelike about the mage seeking for ancient knowledges"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(cmake/FetchDependencies.cmake)

add_subdirectory(src)

add_executable(TheRuneOfTheEldest src/main.cpp)
target_link_libraries(TheRuneOfTheEldest sources)

enable_testing()
add_executable(tests tests/placeholder.cpp)

add_subdirectory(tests)

include(GoogleTest)
gtest_discover_tests(tests)
