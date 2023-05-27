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

include(cmake/Utility.cmake)

macro(fetchBoost module)
    fetchUrl(Boost.${module} 
             boost_${module}.zip 
             https://github.com/boostorg/${module}/archive/refs/tags/boost-1.82.0.zip)
endmacro()

if(NOT TARGET Boost)
    if((NOT TARGET Boost.Stacktrace_windbg AND NOT TARGET boost_stacktrace_basic) 
            OR NOT TARGET Boost.DI OR NOT TARGET boost_container_hash)
        fetchBoost(throw_exception)
        fetchBoost(predef)
        fetchBoost(core)
        fetchBoost(config)
        fetchBoost(winapi)
        fetchBoost(static_assert)
        fetchBoost(assert)
        fetchBoost(array)
        fetchBoost(type_traits)
        fetchBoost(mp11)
        fetchBoost(describe)

        fetchBoost(container_hash)
        if(NOT TARGET boost_container_hash)
            message(FATAL_ERROR "Failed FetchContent: Boost.ContainerHash")
        endif()

        fetchBoost(stacktrace)
        if(NOT TARGET boost_stacktrace_windbg AND NOT TARGET boost_stacktrace_basic)
            message(FATAL_ERROR "Failed FetchContent: Boost.Stacktrace")
        endif()

        fetchUrl(Boost.DI boost_di.zip https://github.com/boost-ext/di/archive/refs/tags/v1.2.0.zip)
        if(NOT TARGET Boost.DI)
            message(FATAL_ERROR "Failed FetchContent: Boost.DI")
        endif()
    endif()

    addDependency(Boost boost_container_hash)
    addDependency(Boost Boost.DI)

    if(TARGET boost_stacktrace_windbg)
        addDependency(Boost boost_stacktrace_windbg)
    else()
        addDependency(Boost boost_stacktrace_basic)
    endif()
endif()

addDependency(dependencies Boost)
