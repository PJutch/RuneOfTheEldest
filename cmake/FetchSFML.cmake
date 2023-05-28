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

if(NOT TARGET SFML)
    if(    NOT TARGET sfml-system 
        OR NOT TARGET sfml-window 
        OR NOT TARGET sfml-graphics 
        OR NOT TARGET sfml-audio 
        OR NOT TARGET sfml-main)

        set(SFML_STATIC_LIBRARIES TRUE)
        include(cmake/Utility.cmake)
        fetchGit(SFML SFML.zip https://github.com/SFML/SFML origin/2.6.x)
    endif()

    addDependency(SFML sfml-system)
    addDependency(SFML sfml-window)
    addDependency(SFML sfml-graphics)
    addDependency(SFML sfml-audio)

    if(WIN32)
        addDependency(SFML sfml-main)
    endif()
endif()

addDependency(dependencies SFML)
