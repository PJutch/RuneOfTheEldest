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

include(FetchContent)

set(CMAKE_PREFIX_PATH deps/googletest-build/googletest/generated
                      deps/sfml-build)

if(NOT TARGET GTest::gtest_main)
    FetchContent_Declare(
        googletest
        # URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG origin/v1.13.x
        DOWNLOAD_EXTRACT_TIMESTAMP true
        # FIND_PACKAGE_ARGS NAMES GTest
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    if(NOT TARGET GTest::gtest_main)
        message(FATAL_ERROR "Failed FetchContent: googletest")
    endif()
endif()

target_link_libraries(test_dependencies INTERFACE GTest::gtest_main)

if(NOT TARGET spdlog)
    FetchContent_Declare(
        spdlog
        URL https://github.com/gabime/spdlog/archive/refs/tags/v1.11.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )   
    set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(spdlog)

    if(NOT TARGET spdlog)
        message(FATAL_ERROR "Failed FetchContent: spdlog")
    endif()
endif()

target_link_libraries(dependencies INTERFACE spdlog)

if(NOT TARGET Boost)
    include(cmake/FetchBoost.cmake)
endif()
target_link_libraries(dependencies INTERFACE Boost)

if(NOT TARGET SFML)
    include(cmake/FetchSFML.cmake)
endif()
target_link_libraries(dependencies INTERFACE SFML)
