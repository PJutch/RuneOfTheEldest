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

#include "Level.hpp"

bool Level::isValidRect(sf::IntRect rect) const noexcept {
    return isValidX(rect.left) && isValidY(rect.top)
        && rect.width >= 0 && rect.height >= 0
        && isValidX(rect.left + rect.width) 
        && isValidY(rect.top + rect.height);
}

void Level::generateBlank(sf::Vector2i newShape) {
    shape_ = newShape;

    tiles.clear();
    tiles.resize(shape().x * shape().y, Tile::UNSEEN);
}

void Level::generateWalls() {
    for (int x = 0; x < shape().x; ++ x)
        for (int y = 0; y < shape().y; ++ y)
            if (at(x, y) == Tile::UNSEEN) {
                bool foundWall = false;
                for (int dx = -1; dx <= 1 && !foundWall; ++ dx)
                    for (int dy = -1; dy <= 1 && !foundWall; ++ dy)
                        if ((dx != 0 || dy != 0) 
                         && bounds().contains(x + dx, y + dy))
                            if (at(x + dx, y + dy) == Tile::EMPTY)
                                foundWall = true;
                
                if (foundWall)
                    at(x, y) = Tile::WALL;
            }
                            
}

sf::Vector2i Level::randomEmptyPosition(RandomEngine& engine) const {
    sf::Vector2i position;
    do {
        position.x = std::uniform_int_distribution{ 0, shape().x - 1 }(engine);
        position.y = std::uniform_int_distribution{ 0, shape().y - 1 }(engine);
    } while (at(position.x, position.y) != Tile::EMPTY);
    return position;
}
