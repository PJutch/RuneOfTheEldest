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

if((NOT TARGET Boost.Stacktrace_windbg AND NOT TARGET boost_stacktrace_basic) 
        OR NOT TARGET Boost.DI OR NOT TARGET boost_container_hash)
    FetchContent_Declare(
        Boost.ThrowException
        URL https://github.com/boostorg/throw_exception/archive/refs/tags/boost-1.82.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.ThrowException) 

    FetchContent_Declare(
        Boost.Predef
        URL https://github.com/boostorg/predef/archive/refs/tags/boost-1.82.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.Predef) 

    FetchContent_Declare(
        Boost.Core
        URL https://github.com/boostorg/core/archive/refs/tags/boost-1.82.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.Core) 

    FetchContent_Declare(
        Boost.Config
        URL https://github.com/boostorg/config/archive/refs/tags/boost-1.82.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.Config) 

    FetchContent_Declare(
        Boost.Winapi
        URL https://github.com/boostorg/winapi/archive/refs/tags/boost-1.82.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.Winapi) 

    FetchContent_Declare(
        Boost.StaticAssert
        URL https://github.com/boostorg/static_assert/archive/refs/tags/boost-1.82.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.StaticAssert) 

    FetchContent_Declare(
        Boost.Assert
        URL https://github.com/boostorg/assert/archive/refs/tags/boost-1.82.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.Assert) 

    FetchContent_Declare(
        Boost.Array
        URL https://github.com/boostorg/array/archive/refs/tags/boost-1.82.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.Array) 

    FetchContent_Declare(
        Boost.TypeTraits
        URL https://github.com/boostorg/type_traits/archive/refs/tags/boost-1.82.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.TypeTraits) 

    FetchContent_Declare(
        Boost.Mp11
        URL https://github.com/boostorg/mp11/archive/refs/tags/boost-1.82.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.Mp11) 

    FetchContent_Declare(
        Boost.Describe
        URL https://github.com/boostorg/describe/archive/refs/tags/boost-1.82.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.Describe) 

    FetchContent_Declare(
        Boost.ContainerHash
        URL https://github.com/boostorg/container_hash/archive/refs/tags/boost-1.82.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.ContainerHash) 

    if(NOT TARGET boost_container_hash)
        message(FATAL_ERROR "Failed FetchContent: Boost.ContainerHash")
    endif()

    FetchContent_Declare(
        Boost.Stacktrace
        URL https://github.com/boostorg/stacktrace/archive/refs/tags/boost-1.82.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.Stacktrace) 

    if(NOT TARGET boost_stacktrace_windbg AND NOT TARGET boost_stacktrace_basic)
        message(FATAL_ERROR "Failed FetchContent: Boost.Stacktrace")
    endif()


    FetchContent_Declare(
        Boost.DI
        URL https://github.com/boost-ext/di/archive/refs/tags/v1.2.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
    )
    FetchContent_MakeAvailable(Boost.DI)  

    if(NOT TARGET Boost.DI)
        message(FATAL_ERROR "Failed FetchContent: Boost.DI")
    endif()
endif()

add_library(Boost INTERFACE)
target_link_libraries(Boost INTERFACE boost_container_hash Boost.DI)

if(TARGET boost_stacktrace_windbg)
    target_link_libraries(Boost INTERFACE boost_stacktrace_windbg)
else()
    target_link_libraries(Boost INTERFACE boost_stacktrace_basic)
endif()
