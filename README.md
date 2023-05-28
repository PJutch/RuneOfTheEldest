# TheRuneOfTheEldest

## About
A rogulike about the mage seeking for ancient knowledge. 
Currently WIP.

## Licence
This project is licensed under GNU GPL 3. See COPYING file for more info.

## Dependencies
Dependencies are automaticaly downloaded on configuration step with FetchContent
If you have problems with this, you can create `deps` directory and place manually downloaded archives there.

This project is dependent on following libraries and their dependencies:
- [SFML](https://github.com/SFML/SFML)
- [Boost](https://www.boost.org) 
    - [Stacktrace](https://www.boost.org/doc/libs/1_82_0/doc/html/stacktrace.html)
    - [ContainerHash](https://www.boost.org/doc/libs/1_82_0/libs/container_hash/doc/html/hash.html)
- [Boost.DI](https://github.com/boost-ext/di)
- [spdlog](https://github.com/gabime/spdlog) (std::format used instead of fmt)
- [GoogleTest](https://github.com/google/googletest)

Also you will need [Doxygen](https://github.com/doxygen/doxygen) to generate documentation

## Platforms
This project is tested on Windows 10.
It should work on other platforms including older versions of Windows, Linux and MacOS 
but it isn't tested.

## Build
This project is build with CMake.

Build targets are:
- `TheRuneOfTheEldest` - main application
- `tests` - GoogleTest tests
- `doc` - documentation generation by [Doxygen](https://github.com/doxygen/doxygen)
