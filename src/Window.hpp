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

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

class Title {
public:
    explicit Title(std::string title) : title_{std::move(title)} {}

    operator std::string() const noexcept {
        return title_;
    }

    operator sf::String() const noexcept {
        return sf::String::fromUtf8(title_.begin(), title_.end());
    }
private:
    std::string title_;
};

class Style {
public:
    explicit Style(sf::Uint32 style) : style_{style} {}

    operator sf::Uint32() const noexcept {
        return style_;
    }
private:
    sf::Uint32 style_;
};

class Window : public sf::RenderWindow {
public:
    Window(sf::VideoMode mode, const Title& title, Style style) : 
        sf::RenderWindow{mode, title, style} {}
};
