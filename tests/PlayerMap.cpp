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

#include "render/PlayerMap.hpp"

#include "core/World.hpp"
#include "core/Actor.hpp"

#include "util/assert.hpp"

#include <gtest/gtest.h>

#include <memory>

namespace {
    class TestController : public core::Controller {
    public:
        TestController() : Controller{"test"} {}

        bool act() final {
            return true;
        }
    };

    auto testXpManager = std::make_shared<core::XpManager>();

    std::shared_ptr<core::Actor> makeTestActor(sf::Vector3i pos) {
        auto actor = std::make_shared<core::Actor>(core::Actor::Stats{ .maxHp = 1 }, pos, nullptr, testXpManager, nullptr, nullptr);
        actor->controller(std::make_unique<TestController>());
        return std::move(actor);
    }

    std::shared_ptr<core::Actor> makeTestActor() {
        return makeTestActor({ 0, 0, 0 });
    }
}

TEST(PlayerMap, tileVisibilityEmpty) {
    auto world = std::make_shared<core::World>();
    world->tiles().assign({ 3, 3, 1 }, core::Tile::EMPTY);

    auto player = makeTestActor({ 0, 2, 0 });
    world->player(std::move(player));

    render::PlayerMap playerMap{ world, std::make_shared<util::Raycaster>(world) };
    playerMap.onGenerate();
    playerMap.update();

    for (int x = 0; x < 3; ++x)
        for (int y = 0; y < 3; ++y)
            EXPECT_EQ(playerMap.tileState({ x, y, 0 }), render::PlayerMap::TileState::VISIBLE);
}

namespace {
    std::shared_ptr<core::World> createWallWorld() {
        auto world = std::make_shared<core::World>();
        world->tiles().assign({ 3, 3, 1 }, core::Tile::EMPTY);

        for (int x = 0; x < 3; ++x)
            world->tiles()[{x, 1, 0}] = core::Tile::WALL;

        auto player = makeTestActor();
        world->player(std::move(player));

        return std::move(world);
    }
}

TEST(PlayerMap, tileVisibilityWall) {
    auto world = createWallWorld();
    world->player().position({ 1, 0, 0 });

    render::PlayerMap playerMap{ world, std::make_shared<util::Raycaster>(world) };
    playerMap.onGenerate();
    playerMap.update();

    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(playerMap.tileState({ x, 0, 0 }), render::PlayerMap::TileState::VISIBLE);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(playerMap.tileState({ x, 1, 0 }), render::PlayerMap::TileState::VISIBLE);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(playerMap.tileState({ x, 2, 0 }), render::PlayerMap::TileState::UNSEEN);
}

TEST(PlayerMap, tileMemorization) {
    auto world = createWallWorld();

    render::PlayerMap playerMap{ world, std::make_shared<util::Raycaster>(world) };
    playerMap.onGenerate();

    world->player().position({ 1, 0, 0 });
    playerMap.update();
    world->player().position({ 1, 2, 0 });
    playerMap.update();

    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(playerMap.tileState({ x, 0, 0 }), render::PlayerMap::TileState::MEMORIZED);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(playerMap.tileState({ x, 1, 0 }), render::PlayerMap::TileState::VISIBLE);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(playerMap.tileState({ x, 2, 0 }), render::PlayerMap::TileState::VISIBLE);
}

TEST(PlayerMap, seenActors) {
    auto world = createWallWorld();
    world->player().position({ 1, 0, 0 });

    world->addActor(makeTestActor({ 2, 0, 0 }));
    world->addActor(makeTestActor({ 2, 2, 0 }));

    render::PlayerMap playerMap{ world, std::make_shared<util::Raycaster>(world) };
    playerMap.onGenerate();
    playerMap.update();

    EXPECT_EQ(std::ssize(playerMap.seenActors()), 1);
    EXPECT_EQ(playerMap.seenActors()[0].position, (sf::Vector3i{ 2, 0, 0 }));
}

TEST(PlayerMap, seenActorsMemorization) {
    auto world = createWallWorld();
    auto actor = makeTestActor({ 2, 0, 0 });
    world->addActor(actor);

    render::PlayerMap playerMap{ world, std::make_shared<util::Raycaster>(world) };
    playerMap.onGenerate();

    world->player().position({ 1, 0, 0 });
    playerMap.update();
    world->player().position({ 1, 2, 0 });
    playerMap.update();
    actor->position({1, 0, 0});

    EXPECT_EQ(std::ssize(playerMap.seenActors()), 1);
    EXPECT_EQ(playerMap.seenActors()[0].position, (sf::Vector3i{ 2, 0, 0 }));
}
