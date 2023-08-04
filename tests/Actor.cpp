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

#include "core/Actor.hpp"

#include <gtest/gtest.h>

namespace {
	class TestController : public core::Controller {
	public:
		bool act() final {
			return true;
		}

		void wantsSwap(bool newWantsSwap) noexcept {
			Controller::wantsSwap(newWantsSwap);
		}

		void handleSwap() noexcept final {
			hadSwapped_ = true;
		}

		bool hadSwapped() const noexcept {
			return hadSwapped_;
		}
	private:
		bool hadSwapped_ = false;
	};

	auto testXpManager = std::make_shared<core::XpManager>();

	core::Actor makeTestActor(double maxHp, double regen, double damage, double turnDelay) {
		return core::Actor{ core::Actor::Stats{ maxHp, regen, damage, turnDelay, 0, nullptr }, nullptr, testXpManager, nullptr };
	}

	std::shared_ptr<core::Actor> makeSharedTestActor(double maxHp, double regen, double damage, double turnDelay) {
		return std::make_shared<core::Actor>(makeTestActor(maxHp, regen, damage, turnDelay));
	}

	std::shared_ptr<core::Actor> makeSharedTestActor(sf::Vector3i pos, std::shared_ptr<core::World> world) {
		return std::make_shared<core::Actor>(core::Actor::Stats{ .maxHp = 1 }, pos, std::move(world), testXpManager, nullptr );
	}
}

TEST(Actor, initialHp) {
	core::Actor actor = makeTestActor(5.0, 1.0, 1.0, 1);

	EXPECT_TRUE(actor.isAlive());
	EXPECT_EQ(actor.hp(), 5);
}

TEST(Actor, beDamaged) {
	core::Actor actor = makeTestActor(5.0, 1.0, 1.0, 1);

	actor.beDamaged(3);

	EXPECT_TRUE(actor.isAlive());
	EXPECT_EQ(actor.hp(), 2);
}

TEST(Actor, beDamagedLethal) {
	core::Actor actor = makeTestActor(5.0, 1.0, 1.0, 1);

	actor.beDamaged(7);

	EXPECT_FALSE(actor.isAlive());
}

TEST(Actor, regen) {
	core::Actor actor = makeTestActor(5.0, 1.0, 1.0, 2);

	actor.beDamaged(3);
	actor.endTurn();

	EXPECT_TRUE(actor.isAlive());
	EXPECT_EQ(actor.hp(), 4);
}

TEST(Actor, regenLimit) {
	core::Actor actor = makeTestActor(5.0, 1.0, 1.0, 7);

	actor.beDamaged(3);
	actor.endTurn();

	EXPECT_TRUE(actor.isAlive());
	EXPECT_EQ(actor.hp(), 5);
}

auto createAliveActorTryMoveToTest() {
	auto world = std::make_shared<core::World>();
	world->tiles().assign({ 3, 3, 1 }, Tile::EMPTY);

	world->tiles()[{ 1, 0, 0 }] = Tile::WALL;
	world->tiles()[{ 1, 1, 0 }] = Tile::WALL;

	auto actor = makeSharedTestActor({ 0, 1, 0 }, world);
	actor->controller(std::make_unique<TestController>());
	world->addActor(actor);

	auto other = makeSharedTestActor({ 0, 2, 0 }, world);
	other->controller(std::make_unique<TestController>());
	world->addActor(other);

	return std::pair{ std::move(actor), std::move(other) };
}

TEST(Actor, tryMoveToEmpty) {
	auto [actor, other] = createAliveActorTryMoveToTest();

	EXPECT_TRUE(actor->canMoveToOrAttack({ 1, 2, 0 }, false));
	actor->tryMoveTo({1, 2, 0}, false);
	EXPECT_EQ(actor->position(), (sf::Vector3i{1, 2, 0}));
}

TEST(Actor, tryMoveToWall) {
	auto [actor, other] = createAliveActorTryMoveToTest();

	EXPECT_FALSE(actor->canMoveToOrAttack({ 1, 0, 0 }, false));
	actor->tryMoveTo({ 1, 0, 0 }, false);
	EXPECT_EQ(actor->position(), (sf::Vector3i{ 0, 1, 0 }));
}

TEST(Actor, tryMoveToSwap) {
	auto [actor, other] = createAliveActorTryMoveToTest();

	EXPECT_TRUE(actor->canMoveToOrAttack({ 0, 2, 0 }, false));

	actor->tryMoveTo({ 0, 2, 0 }, false);
	EXPECT_EQ(actor->position(), (sf::Vector3i{ 0, 2, 0 }));
	EXPECT_EQ(other->position(), (sf::Vector3i{ 0, 1, 0 }));
	EXPECT_TRUE(dynamic_cast<TestController&>(actor->controller()).hadSwapped());
	EXPECT_TRUE(dynamic_cast<TestController&>(other->controller()).hadSwapped());
}

TEST(Actor, tryMoveToDontSwap) {
	auto [actor, other] = createAliveActorTryMoveToTest();
	dynamic_cast<TestController&>(other->controller()).wantsSwap(false);

	EXPECT_FALSE(actor->canMoveToOrAttack({ 0, 2, 0 }, false));

	actor->tryMoveTo({ 0, 2, 0 }, false);
	EXPECT_EQ(actor->position(), (sf::Vector3i{ 0, 1, 0 }));
	EXPECT_EQ(other->position(), (sf::Vector3i{ 0, 2, 0 }));
	EXPECT_FALSE(dynamic_cast<TestController&>(actor->controller()).hadSwapped());
	EXPECT_FALSE(dynamic_cast<TestController&>(other->controller()).hadSwapped());
}

TEST(Actor, tryMoveToForceSwap) {
	auto [actor, other] = createAliveActorTryMoveToTest();
	dynamic_cast<TestController&>(other->controller()).wantsSwap(false);

	EXPECT_TRUE(actor->canMoveToOrAttack({ 0, 2, 0 }, true));

	actor->tryMoveTo({ 0, 2, 0 }, true);
	EXPECT_EQ(actor->position(), (sf::Vector3i{ 0, 2, 0 }));
	EXPECT_EQ(other->position(), (sf::Vector3i{ 0, 1, 0 }));	
	EXPECT_TRUE(dynamic_cast<TestController&>(actor->controller()).hadSwapped());
	EXPECT_TRUE(dynamic_cast<TestController&>(other->controller()).hadSwapped());
}
