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

#include "line.hpp"

#include "core/World.hpp"

#include <numeric>

namespace generation {
    namespace {
        void randomFill(core::World& world, Area area, double density,
                        util::RandomEngine& randomEngine, bool debugTiles) {
            for (int x = area.left(); x < area.right() - 1; ++x)
                for (int y = area.top(); y < area.bottom() - 1; ++y)
                    if (std::bernoulli_distribution{density}(randomEngine))
                        world.tiles()[{x, y, area.z()}] = (debugTiles ? core::Tile::ROOM : core::Tile::EMPTY);
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
                    if (world.tiles().isValidPosition(cur) && world.tiles()[cur] != core::Tile::WALL)
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
                            world.tiles()[{x, y, area.z()}] = (debugTiles ? core::Tile::ROOM : core::Tile::EMPTY);
                        else
                            world.tiles()[{x, y, area.z()}] = core::Tile::WALL;
            }
        }

        struct ComponentData {
            ComponentData(sf::IntRect bounds) : ids{bounds} {}

            std::vector<int> sizes;
            Buffer ids;
        };

        void markComponent(const core::World& world, sf::IntRect bounds, sf::Vector3i current, int id, Buffer& ids, int& size) {
            if (!bounds.contains(util::getXY(current))
             || world.tiles()[current] != core::Tile::EMPTY
             || ids[util::getXY(current)] != 0)
                return;

            ++size;
            ids[util::getXY(current)] = id;
            for (int dx = -1; dx <= 1; ++dx)
                for (int dy = -1; dy <= 1; ++dy) {
                    sf::Vector3i next{current.x + dx, current.y + dy, current.z};
                    markComponent(world, bounds, next, id, ids, size);
                }
        }

        ComponentData findComponents(const core::World& world, Area area) {
            ComponentData res{area.bounds()};
            for (int x = area.left(); x < area.right() - 1; ++x)
                for (int y = area.top(); y < area.bottom() - 1; ++y)
                    if (world.tiles()[{x, y, area.z()}] == core::Tile::EMPTY && res.ids[{x, y}] == 0) {
                        res.sizes.emplace_back();
                        markComponent(world, area.bounds(), {x, y, area.z()},
                                       std::ssize(res.sizes), res.ids, res.sizes.back());
                    }
            return res;
        }

        void paintDebugComponents(core::World& world, Area area, const Buffer& ids) {
            for (int x = area.left(); x < area.right() - 1; ++x)
                for (int y = area.top(); y < area.bottom() - 1; ++y)
                    if (world.tiles()[{x, y, area.z()}] == core::Tile::EMPTY)
                        world.tiles()[{x, y, area.z()}] = static_cast<core::Tile>(static_cast<int>(core::Tile::COMPONENT1) + ids[{x, y}] % 3);
        }

        template <typename Range>
        bool allSame(const Range& range) {
            if (range.begin() == range.end())
                return true;

            return std::ranges::all_of(range, [&range](const auto& elem) {
                return elem == *range.begin();
            });
        }

        sf::Vector3i randomPosIn(const core::World& world, Area area, int id, const Buffer& ids, const std::vector<int>& aliases) {
            return world.randomPositionAt(area.z(), [&](const core::World&, sf::Vector3i pos) {
                return area.bounds().contains(util::getXY(pos))
                    && ids[util::getXY(pos)] != 0
                    && aliases[ids[util::getXY(pos)] - 1] == id;
            });
        }

        sf::Vector3i randomPosIn(const core::World& world, Area area) {
            return world.randomPositionAt(area.z(), [&](const core::World&, sf::Vector3i pos) {
                return area.bounds().contains(util::getXY(pos));
            });
        }

        void connectHVH(sf::Vector3i pos1, sf::Vector3i pos2, core::World& world, Area area,
                        util::RandomEngine& randomEngine, bool debugTiles) {
            int turnX = std::uniform_int_distribution{std::min(pos1.x, pos2.x), std::max(pos1.x, pos2.x)}(randomEngine);
            horizontalLineInclusive(world, area.z(), pos1.x, turnX , pos1.y, debugTiles ? core::Tile::PASSAGE : core::Tile::EMPTY);
            verticalLineInclusive  (world, area.z(), pos1.y, pos2.y, turnX , debugTiles ? core::Tile::PASSAGE : core::Tile::EMPTY);
            horizontalLineInclusive(world, area.z(), turnX , pos2.x, pos2.y, debugTiles ? core::Tile::PASSAGE : core::Tile::EMPTY);
        }

        void connectVHV(sf::Vector3i pos1, sf::Vector3i pos2, core::World& world, Area area,
                        util::RandomEngine& randomEngine, bool debugTiles) {
            int turnY = std::uniform_int_distribution{std::min(pos1.y, pos2.y), std::max(pos1.y, pos2.y)}(randomEngine);
            verticalLineInclusive(world, area.z(), pos1.y, turnY , pos1.x, debugTiles ? core::Tile::PASSAGE : core::Tile::EMPTY);
            verticalLineInclusive(world, area.z(), pos1.x, pos2.x, turnY , debugTiles ? core::Tile::PASSAGE : core::Tile::EMPTY);
            verticalLineInclusive(world, area.z(), turnY , pos2.y, pos2.x, debugTiles ? core::Tile::PASSAGE : core::Tile::EMPTY);
        }

        void connect(sf::Vector3i pos1, sf::Vector3i pos2, core::World& world, Area area, 
                     util::RandomEngine& randomEngine, bool debugTiles) {
            if (std::bernoulli_distribution{0.5}(randomEngine))
                connectHVH(pos1, pos2, world, area, randomEngine, debugTiles);
            else
                connectVHV(pos1, pos2, world, area, randomEngine, debugTiles);
        }

        template <typename T>
        ptrdiff_t randomIndex(std::vector<T> vec, util::RandomEngine& randomEngine) {
            return std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(vec) - 1}(randomEngine);
        }

        void connectComponents(ComponentData components, core::World& world, Area area,
                               util::RandomEngine& randomEngine, bool debugTiles) {
            std::vector<int> aliases(std::ssize(components.sizes));
            std::iota(aliases.begin(), aliases.end(), 0);

            while (!allSame(aliases)) {
                int i = randomIndex(aliases, randomEngine);
                int j = randomIndex(aliases, randomEngine);

                if (aliases[i] == aliases[j])
                    continue;

                connect(randomPosIn(world, area, aliases[i], components.ids, aliases),
                        randomPosIn(world, area, aliases[j], components.ids, aliases),
                        world, area, randomEngine, debugTiles);

                for (int& alias : aliases)
                    if (alias == aliases[j])
                        alias = aliases[i];
            }
        }

        void connectPassages(core::World& world, Area area, util::RandomEngine& randomEngine, bool debugTiles) {
            for (int y : area.leftPassages())
                connect({area.left(), y, area.z()}, randomPosIn(world, area),
                        world, area, randomEngine, debugTiles);

            for (int y : area.rightPassages())
                connect({area.right() - 1, y, area.z()}, randomPosIn(world, area),
                    world, area, randomEngine, debugTiles);

            for (int x : area.topPassages())
                connect({x, area.top(), area.z()}, randomPosIn(world, area),
                    world, area, randomEngine, debugTiles);

            for (int x : area.bottomPassages())
                connect({x, area.bottom() - 1, area.z()}, randomPosIn(world, area),
                    world, area, randomEngine, debugTiles);
        }
    }

    void cave(core::World& world, Area area, util::RandomEngine& randomEngine, bool debugTiles) {
        randomFill(world, area, 0.5, randomEngine, debugTiles);
        smooth(world, area, 3, debugTiles);

        auto components = findComponents(world, area);

        const bool debugComponents = false;
        if (debugComponents)
            paintDebugComponents(world, area, components.ids);

        connectComponents(components, world, area, randomEngine, debugTiles);
        connectPassages(world, area, randomEngine, debugTiles);
    }
}
