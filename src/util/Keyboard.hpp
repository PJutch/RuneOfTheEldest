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

#ifndef KEYBOARD_HPP_
#define KEYBOARD_HPP_

/// @file event.hpp Utilities for sf::Keyboard

#include <SFML/Window.hpp>

namespace util {
    /// Checks if event is KeyPressed event for key key
    inline bool wasKeyPressed(sf::Event event, sf::Keyboard::Key key) noexcept {
        return event.type == sf::Event::KeyPressed && event.key.code == key;
    }

    /// Gets numpad key for number
    inline sf::Keyboard::Key numpad(int i) noexcept {
        return static_cast<sf::Keyboard::Key>(sf::Keyboard::Numpad0 + i);
    }

    /// Gets number for numpad key
    inline int fromNumpad(sf::Keyboard::Key numpad_) noexcept {
        return numpad_ - sf::Keyboard::Numpad0;
    }

    ///  Chacks if key is numpad key
    inline bool isNumpad(sf::Keyboard::Key key) noexcept {
        return sf::Keyboard::Numpad0 <= key && key <= sf::Keyboard::Numpad9;
    }
}

#endif