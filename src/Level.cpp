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

#include <queue>

void Level::generate(sf::Vector2i newShape, sf::IntRect room) {
    generateBlank(newShape);
    generateDungeon();
}

void Level::generateBlank(sf::Vector2i newShape) {
    shape_ = newShape;

    tiles.clear();
    tiles.resize(shape().x * shape().y, Tile::UNSEEN);
}

void Level::generateDungeon() {
    const int minSize = 5;

    std::queue<sf::IntRect> areas;
    areas.push({0, 0, shape().x, shape().y});

    while (!areas.empty()) {
        sf::IntRect area = areas.front();
        areas.pop();

        if (area.width <= 2 * minSize && area.height <= 2 * minSize 
            || std::uniform_real_distribution{}(*randomEngine) < 0.1)
            generateRoom(area);
        else {
            if (area.width > 2 * minSize && (area.height <= 2 * minSize 
             || std::uniform_real_distribution{}(*randomEngine) < 0.5)) {
                int width1 = std::uniform_int_distribution
                    {minSize, area.width - minSize}(*randomEngine);
                areas.emplace(area.left, area.top, width1, area.height);

                int left2 = area.left + width1;
                int width2 = area.width - width1;
                areas.emplace(left2, area.top, width2, area.height);
            } else {
                int height1 = std::uniform_int_distribution
                    {minSize, area.height - minSize}(*randomEngine);
                areas.emplace(area.left, area.top, area.width, height1);

                int top2 = area.top + height1;
                int height2 = area.height - height1;
                areas.emplace(area.left, top2, area.width, height2);
            }
        }
    }
}

void Level::generateRoom(sf::IntRect room) {
    for (int x = room.left; x < room.left + room.width; ++ x) {
        at(x, room.top) = Tile::WALL;
        at(x, room.top + room.height - 1) = Tile::WALL;
    }

    for (int y = room.top; y < room.top + room.height; ++ y) {
        at(room.left, y) = Tile::WALL;
        at(room.left + room.width - 1, y) = Tile::WALL;
    }

    for (int x = room.left + 1; x < room.left + room.width - 1; ++ x)
        for (int y = room.top + 1; y < room.top + room.height - 1; ++ y)
            at(x, y) = Tile::EMPTY;
}
