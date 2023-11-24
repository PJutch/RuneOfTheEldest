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

#include "cave.hpp"

#include "core/World.hpp"

namespace generation {
    namespace {
        void randomFill(core::World& world, Area area, double density,
                        util::RandomEngine& randomEngine, bool debugTiles) {
            for (int x = area.left(); x < area.right() - 1; ++x)
                for (int y = area.top(); y < area.bottom() - 1; ++y)
                    if (std::bernoulli_distribution{density}(randomEngine))
                        world.tiles()[{x, y, area.z()}] = (debugTiles ? Tile::ROOM : Tile::EMPTY);
        }

        class Buffer {
        public:
            Buffer(sf::IntRect bounds_) noexcept : 
                bounds{bounds_}, data(bounds.width * bounds.height) {}

            int& operator[] (sf::Vector2i pos) noexcept {
                return data[index(pos)];
            }

            const int& operator[] (sf::Vector2i pos) const noexcept {
                return data[index(pos)];
            }
        private:
            sf::IntRect bounds;
            std::vector<int> data;

            int index(sf::Vector2i pos) const noexcept {
                return (pos.x - bounds.left) * bounds.height + pos.y - bounds.top;
            }
        };

        int empty_neighbours(core::World& world, sf::Vector3i p) {
            int res = 0;
            for (int dx = -1; dx <= 1; ++dx)
                for (int dy = -1; dy <= 1; ++dy) {
                    sf::Vector3i cur{p.x + dx, p.y + dy, p.z};
                    if (world.tiles().isValidPosition(cur) && world.tiles()[cur] != Tile::WALL)
                        ++res;
                }
            return res;
        }

        void smooth(core::World& world, Area area, int steps, bool debugTiles) {
            Buffer buffer{area.bounds()};
            for (int i = 0; i < steps; ++i) {
                for (int x = area.left(); x < area.right() - 1; ++x)
                    for (int y = area.top(); y < area.bottom() - 1; ++y)
                        buffer[{x, y}] = empty_neighbours(world, {x, y, area.z()});

                for (int x = area.left(); x < area.right() - 1; ++x)
                    for (int y = area.top(); y < area.bottom() - 1; ++y)
                        if (buffer[{x, y}] >= 5)
                            world.tiles()[{x, y, area.z()}] = (debugTiles ? Tile::ROOM : Tile::EMPTY);
                        else
                            world.tiles()[{x, y, area.z()}] = Tile::WALL;
            }
        }
    }

    void cave(core::World& world, Area area, util::RandomEngine& randomEngine, bool debugTiles) {
        randomFill(world, area, 0.5, randomEngine, debugTiles);
        smooth(world, area, 3, debugTiles);
    }
}
