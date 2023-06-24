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

#include "SeenTiles.hpp"

#include "World.hpp"
#include "Player.hpp"

#include <gtest/gtest.h>

#include <memory>

namespace { 
    class TestActor : public Actor {
    public:
        TestActor() = default;
        TestActor(sf::Vector3i newPosition) noexcept : position_{ newPosition }{}

        sf::Vector3i position() const noexcept final {
            return position_;
        }

        void position(sf::Vector3i newPosition) noexcept final {
            position_ = newPosition;
        }

        bool act() override {
            return true;
        }

        int nextTurn() const noexcept final {
            return 0;
        }

        bool isAlive() const noexcept final {
            return true;
        }

        bool shouldInterruptOnDelete() const noexcept final {
            return false;
        }

        void beDamaged(double damage) noexcept final {}

        class DrawMemento : public Actor::DrawMemento {
        public:
            DrawMemento(const TestActor& actor) : position_{ actor.position() } {}

            [[nodiscard]] sf::Vector3i position() const noexcept final {
                return position_;
            }

            void draw(Renderer& renderer) const noexcept final {}
        private:
            sf::Vector3i position_{ 0, 0, 0 };
        };

        std::unique_ptr<Actor::DrawMemento> createDrawMemento() const noexcept final {
            return std::make_unique<DrawMemento>(*this);
        }

        [[nodiscard]] bool isOnPlayerSide() const final {
            return false;
        }

        bool wantsSwap() const noexcept final {
            return true;
        }

        void handleSwap() noexcept final {}

        void handleSound(Sound sound) noexcept final {}
    private:
        sf::Vector3i position_{ 0, 0, 0 };
    };
}

TEST(SeenTiles, tileVisibilityEmpty) {
    auto dungeon = std::make_shared<Dungeon>();
    dungeon->resize(1);
    (*dungeon)[0].generateBlank({ 3, 3 });

    for (int x = 0; x < 3; ++x)
        for (int y = 0; y < 3; ++y)
            dungeon->at(x, y, 0) = Tile::EMPTY;

    auto player = std::make_shared<Player>();
    player->position({ 0, 2, 0 });

    SeenTiles seenTiles{ std::move(player), std::make_shared<World>(std::move(dungeon)) };
    seenTiles.onGenerate();
    seenTiles.update();

    for (int x = 0; x < 3; ++x)
        for (int y = 0; y < 3; ++y)
            EXPECT_EQ(seenTiles.tileState({ x, y, 0 }), SeenTiles::TileState::VISIBLE);
}

namespace {
    std::shared_ptr<World> createWallWorld() {
        auto dungeon = std::make_shared<Dungeon>();
        dungeon->resize(1);
        (*dungeon)[0].generateBlank({ 3, 3 });

        for (int x = 0; x < 3; ++x)
            dungeon->at(x, 0, 0) = Tile::EMPTY;
        for (int x = 0; x < 3; ++x)
            dungeon->at(x, 1, 0) = Tile::WALL;
        for (int x = 0; x < 3; ++x)
            dungeon->at(x, 2, 0) = Tile::EMPTY;

        return std::make_shared<World>(std::move(dungeon));
    }
}

TEST(SeenTiles, tileVisibilityWall) {
    auto player = std::make_shared<Player>();
    player->position({ 1, 0, 0 });

    SeenTiles seenTiles{ std::move(player), createWallWorld() };
    seenTiles.onGenerate();
    seenTiles.update();

    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(seenTiles.tileState({ x, 0, 0 }), SeenTiles::TileState::VISIBLE);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(seenTiles.tileState({ x, 1, 0 }), SeenTiles::TileState::VISIBLE);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(seenTiles.tileState({ x, 2, 0 }), SeenTiles::TileState::UNSEEN);
}

TEST(SeenTiles, tileMemorization) {
    auto player = std::make_shared<Player>();

    SeenTiles seenTiles{ player, createWallWorld() };
    seenTiles.onGenerate();

    player->position({ 1, 0, 0 });
    seenTiles.update();
    player->position({ 1, 2, 0 });
    seenTiles.update();

    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(seenTiles.tileState({ x, 0, 0 }), SeenTiles::TileState::MEMORIZED);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(seenTiles.tileState({ x, 1, 0 }), SeenTiles::TileState::VISIBLE);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(seenTiles.tileState({ x, 2, 0 }), SeenTiles::TileState::VISIBLE);
}

TEST(SeenTiles, seenActors) {
    auto player = std::make_shared<Player>();
    player->position({ 1, 0, 0 });

    auto world = createWallWorld();
    world->addActor(std::make_shared<TestActor>(sf::Vector3i{ 2, 0, 0 }));
    world->addActor(std::make_shared<TestActor>(sf::Vector3i{ 2, 2, 0 }));

    SeenTiles seenTiles{ std::move(player), std::move(world) };
    seenTiles.onGenerate();
    seenTiles.update();

    EXPECT_EQ(std::ssize(seenTiles.seenActors()), 1);
    EXPECT_EQ(seenTiles.seenActors()[0]->position(), (sf::Vector3i{ 2, 0, 0 }));
}

TEST(SeenTiles, seenActorsMemorization) {
    auto player = std::make_shared<Player>();

    auto world = createWallWorld();
    auto actor = std::make_shared<TestActor>(sf::Vector3i{ 2, 0, 0 });
    world->addActor(actor);

    SeenTiles seenTiles{ player, std::move(world) };
    seenTiles.onGenerate();

    player->position({ 1, 0, 0 });
    seenTiles.update();
    player->position({ 1, 2, 0 });
    seenTiles.update();
    actor->position({1, 0, 0});

    EXPECT_EQ(std::ssize(seenTiles.seenActors()), 1);
    EXPECT_EQ(seenTiles.seenActors()[0]->position(), (sf::Vector3i{ 2, 0, 0 }));
}
