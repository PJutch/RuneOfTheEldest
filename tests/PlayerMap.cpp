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

#include "PlayerMap.hpp"

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

TEST(PlayerMap, tileVisibilityEmpty) {
    auto world = std::make_shared<World>();
    world->tiles().assign({ 3, 3, 1 }, Tile::EMPTY);

    auto player = std::make_shared<Player>();
    player->position({ 0, 2, 0 });

    PlayerMap playerMap{ std::move(player), std::move(world) };
    playerMap.onGenerate();
    playerMap.update();

    for (int x = 0; x < 3; ++x)
        for (int y = 0; y < 3; ++y)
            EXPECT_EQ(playerMap.tileState({ x, y, 0 }), PlayerMap::TileState::VISIBLE);
}

namespace {
    std::shared_ptr<World> createWallWorld() {
        auto world = std::make_shared<World>();
        world->tiles().assign({ 3, 3, 1 }, Tile::EMPTY);

        for (int x = 0; x < 3; ++x)
            world->tiles()[{x, 1, 0}] = Tile::WALL;

        return std::move(world);
    }
}

TEST(PlayerMap, tileVisibilityWall) {
    auto player = std::make_shared<Player>();
    player->position({ 1, 0, 0 });

    PlayerMap playerMap{ std::move(player), createWallWorld() };
    playerMap.onGenerate();
    playerMap.update();

    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(playerMap.tileState({ x, 0, 0 }), PlayerMap::TileState::VISIBLE);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(playerMap.tileState({ x, 1, 0 }), PlayerMap::TileState::VISIBLE);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(playerMap.tileState({ x, 2, 0 }), PlayerMap::TileState::UNSEEN);
}

TEST(PlayerMap, tileMemorization) {
    auto player = std::make_shared<Player>();

    PlayerMap playerMap{ player, createWallWorld() };
    playerMap.onGenerate();

    player->position({ 1, 0, 0 });
    playerMap.update();
    player->position({ 1, 2, 0 });
    playerMap.update();

    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(playerMap.tileState({ x, 0, 0 }), PlayerMap::TileState::MEMORIZED);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(playerMap.tileState({ x, 1, 0 }), PlayerMap::TileState::VISIBLE);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(playerMap.tileState({ x, 2, 0 }), PlayerMap::TileState::VISIBLE);
}

TEST(PlayerMap, seenActors) {
    auto player = std::make_shared<Player>();
    player->position({ 1, 0, 0 });

    auto world = createWallWorld();
    world->addActor(std::make_shared<TestActor>(sf::Vector3i{ 2, 0, 0 }));
    world->addActor(std::make_shared<TestActor>(sf::Vector3i{ 2, 2, 0 }));

    PlayerMap playerMap{ std::move(player), std::move(world) };
    playerMap.onGenerate();
    playerMap.update();

    EXPECT_EQ(std::ssize(playerMap.seenActors()), 1);
    EXPECT_EQ(playerMap.seenActors()[0]->position(), (sf::Vector3i{ 2, 0, 0 }));
}

TEST(PlayerMap, seenActorsMemorization) {
    auto player = std::make_shared<Player>();

    auto world = createWallWorld();
    auto actor = std::make_shared<TestActor>(sf::Vector3i{ 2, 0, 0 });
    world->addActor(actor);

    PlayerMap playerMap{ player, std::move(world) };
    playerMap.onGenerate();

    player->position({ 1, 0, 0 });
    playerMap.update();
    player->position({ 1, 2, 0 });
    playerMap.update();
    actor->position({1, 0, 0});

    EXPECT_EQ(std::ssize(playerMap.seenActors()), 1);
    EXPECT_EQ(playerMap.seenActors()[0]->position(), (sf::Vector3i{ 2, 0, 0 }));
}
