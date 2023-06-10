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

#include "Keyboard.hpp"

#include <gtest/gtest.h>

TEST(Keyboard, wasKeyPressedThisKey) {
    sf::Event event{ sf::Event::KeyPressed };
    event.key = sf::Event::KeyEvent{ sf::Keyboard::D, sf::Keyboard::Scancode::D };

    EXPECT_TRUE(wasKeyPressed(event, sf::Keyboard::D));
}

TEST(Keyboard, wasKeyPressedOtherKey) {
    sf::Event event{ sf::Event::KeyPressed };
    event.key = sf::Event::KeyEvent{ sf::Keyboard::C, sf::Keyboard::Scancode::C };

    EXPECT_FALSE(wasKeyPressed(event, sf::Keyboard::D));
}

TEST(Keyboard, wasKeyPressedNotKey) {
    sf::Event event{ sf::Event::MouseMoved };
    event.mouseMove = sf::Event::MouseMoveEvent{ 10, 15 };

    EXPECT_FALSE(wasKeyPressed(event, sf::Keyboard::D));
}

TEST(Keyboard, numpad) {
    EXPECT_EQ(numpad(5), sf::Keyboard::Numpad5);
}

TEST(Keyboard, fromNumpad) {
    EXPECT_EQ(fromNumpad(sf::Keyboard::Numpad5), 5);
}

TEST(Keyboard, isNumpadTrue) {
    EXPECT_TRUE(isNumpad(sf::Keyboard::Numpad5));
}

TEST(Keyboard, isNumpadFalse) {
    EXPECT_FALSE(isNumpad(sf::Keyboard::G));
}
