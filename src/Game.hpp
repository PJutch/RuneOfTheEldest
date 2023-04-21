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

#ifndef GAME_HPP_
#define GAME_HPP_

#include "Level.hpp"
#include "Renderer.hpp"
#include "DungeonGenerator.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <memory>

class Game {
public:
    Game(std::shared_ptr<sf::RenderWindow> window, 
         std::shared_ptr<Level> level, 
         std::unique_ptr<Renderer> renderer,
         DungeonGenerator dungeonGenerator);

    void run();
private:
    std::shared_ptr<sf::RenderWindow> window;
    std::shared_ptr<Level> level;
    std::unique_ptr<Renderer> renderer;
    DungeonGenerator dungeonGenerator;

    void handleEvent(sf::Event event);

    void generateLevel();
};

#endif