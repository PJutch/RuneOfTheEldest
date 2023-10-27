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

macro(fetchGit contentName file repository tag)
    message("Fetching ${contentName} (file ${file}) from ${repository} with tag ${tag}")
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../deps/${file}")
        FetchContent_Declare(
            ${contentName}
            URL ${CMAKE_CURRENT_LIST_DIR}/../deps/${file}
            DOWNLOAD_EXTRACT_TIMESTAMP true
        )
    else()
        FetchContent_Declare(
            ${contentName}
            GIT_REPOSITORY ${repository}
            GIT_TAG ${tag}
            DOWNLOAD_EXTRACT_TIMESTAMP true
        )
    endif()

    FetchContent_MakeAvailable(${contentName})
endmacro()

macro(fetchUrl contentName file url)
    message("Fetching ${contentName} (file ${file}) from ${url}")
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../deps/${file}")
        FetchContent_Declare(
            ${contentName}
            URL ${CMAKE_CURRENT_LIST_DIR}/../deps/${file}
            DOWNLOAD_EXTRACT_TIMESTAMP true
        )
    else()
        FetchContent_Declare(
            ${contentName}
            URL ${url}
            DOWNLOAD_EXTRACT_TIMESTAMP true
        )
    endif()

    FetchContent_MakeAvailable(${contentName})
endmacro()

macro(addDependency dependencyLibrary dependency)
    if(NOT TARGET ${dependency})
        message(FATAL_ERROR "Dependency ${dependency} is not satisfied")
    endif()

    if(NOT TARGET ${dependencyLibrary})
        add_library(${dependencyLibrary} INTERFACE)
    endif()

    target_link_libraries(${dependencyLibrary} INTERFACE ${dependency})
endmacro()
