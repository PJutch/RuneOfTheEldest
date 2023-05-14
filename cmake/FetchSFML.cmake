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

if(    NOT TARGET sfml-system 
    OR NOT TARGET sfml-window 
    OR NOT TARGET sfml-graphics 
    OR NOT TARGET sfml-audio 
    OR NOT TARGET sfml-main)
    set(SFML_STATIC_LIBRARIES TRUE)
    FetchContent_Declare(
        SFML
        URL ${CMAKE_CURRENT_LIST_DIR}/../deps/SFML-2.6.x.zip
        # GIT_REPOSITORY https://github.com/SFML/SFML
        # GIT_TAG origin/2.6.x
        DOWNLOAD_EXTRACT_TIMESTAMP true
        # FIND_PACKAGE_ARGS COMPONENTS system window graphics audio main
    )
    FetchContent_MakeAvailable(SFML)

    if(NOT TARGET sfml-system)
        message(FATAL_ERROR "Failed FetchContent: SFML (System)")
    endif()

    if(NOT TARGET sfml-window)
        message(FATAL_ERROR "Failed FetchContent: SFML (Window)")
    endif()

    if(NOT TARGET sfml-graphics)
        message(FATAL_ERROR "Failed FetchContent: SFML (Graphics)")
    endif()

    if(NOT TARGET sfml-audio)
        message(FATAL_ERROR "Failed FetchContent: SFML (Audio)")
    endif()

    if(NOT TARGET sfml-main)
        message(FATAL_ERROR "Failed FetchContent: SFML (Main)")
    endif()
endif()

add_library(SFML INTERFACE)
target_link_libraries(SFML INTERFACE sfml-system 
                                     sfml-window 
                                     sfml-graphics 
                                     sfml-audio 
                                     sfml-main)
