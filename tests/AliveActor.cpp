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

#include "AliveActor.hpp"

#include <gtest/gtest.h>

class TestAliveActor : public AliveActor {
public:
	TestAliveActor(double newMaxHp, double regen_, sf::Vector3i newPosition, std::shared_ptr<World> newWorld = nullptr) :
		AliveActor{ newMaxHp, regen_, newPosition, std::move(newWorld) } {}
	TestAliveActor(double newMaxHp, double regen_, std::shared_ptr<World> newWorld = nullptr) :
		AliveActor{ newMaxHp, regen_, std::move(newWorld) } {}
	TestAliveActor(sf::Vector3i newPosition, std::shared_ptr<World> newWorld = nullptr) :
		AliveActor{ 1.0, 0.0, newPosition, std::move(newWorld) } {}

	void nextTurn(int newNextTurn) noexcept {
		AliveActor::nextTurn(newNextTurn);
	}

	void wait(int time) noexcept {
		AliveActor::wait(time);
	}

	virtual void attack(Actor& actor) {}

	virtual void moveSucceed() {}

	void tryMoveTo(sf::Vector3i newPosition) {
		AliveActor::tryMoveTo(newPosition);
	}

	void tryMoveTo(sf::Vector2i newPosition) {
		AliveActor::tryMoveTo(newPosition);
	}

	void tryMove(sf::Vector2i offset) {
		AliveActor::tryMoveTo(offset);
	}

	using AliveActor::hp;

	void hp(double newHp) noexcept {
		AliveActor::hp(newHp);
	}

	bool act() noexcept final {
		return true;
	}

	bool shouldInterruptOnDelete() const noexcept final {
		return false;
	}

	void draw(Renderer&) const noexcept final {}

	[[nodiscard]] bool isOnPlayerSide() const noexcept final {
		return true;
	}
};

TEST(AliveActor, initialHp) {
	TestAliveActor actor{5.0, 1.0};

	EXPECT_TRUE(actor.isAlive());
	EXPECT_EQ(actor.hp(), 5);
}

TEST(AliveActor, beDamaged) {
	TestAliveActor actor{ 5.0, 1.0 };

	actor.beDamaged(3);

	EXPECT_TRUE(actor.isAlive());
	EXPECT_EQ(actor.hp(), 2);
}

TEST(AliveActor, beDamagedLethal) {
	TestAliveActor actor{ 5.0, 1.0 };

	actor.beDamaged(7);

	EXPECT_FALSE(actor.isAlive());
}

TEST(AliveActor, regen) {
	TestAliveActor actor{ 5.0, 1.0 };

	actor.beDamaged(3);
	actor.wait(2);

	EXPECT_TRUE(actor.isAlive());
	EXPECT_EQ(actor.hp(), 4);
}

TEST(AliveActor, regenLimit) {
	TestAliveActor actor{ 5.0, 1.0 };

	actor.beDamaged(3);
	actor.wait(7);

	EXPECT_TRUE(actor.isAlive());
	EXPECT_EQ(actor.hp(), 5);
}

auto createAliveActorTryMoveToTest() {
	auto dungeon = std::make_shared<Dungeon>();
	dungeon->resize(1);
	(*dungeon)[0].generateBlank({ 3, 3 });

	dungeon->at({ 0, 0, 0 }) = Tile::EMPTY;
	dungeon->at({ 0, 1, 0 }) = Tile::EMPTY;
	dungeon->at({ 0, 2, 0 }) = Tile::EMPTY;
	dungeon->at({ 1, 0, 0 }) = Tile::WALL;
	dungeon->at({ 1, 1, 0 }) = Tile::UNSEEN;
	dungeon->at({ 1, 2, 0 }) = Tile::EMPTY;

	auto world = std::make_shared<World>(std::move(dungeon));

	auto actor = std::make_shared<TestAliveActor>(sf::Vector3i{ 0, 1, 0 }, world);
	auto other = std::make_shared<TestAliveActor>(sf::Vector3i{ 0, 2, 0 }, world);

	world->addActor(actor);
	world->addActor(other);

	return std::pair{ std::move(actor), std::move(other) };
}

TEST(AliveActor, tryMoveToEmpty) {
	auto [actor, other] = createAliveActorTryMoveToTest();
	actor->tryMoveTo({1, 2, 0});

	EXPECT_EQ(actor->position(), (sf::Vector3i{1, 2, 0}));
}

TEST(AliveActor, tryMoveToWall) {
	auto [actor, other] = createAliveActorTryMoveToTest();
	actor->tryMoveTo({ 1, 0, 0 });

	EXPECT_EQ(actor->position(), (sf::Vector3i{ 0, 1, 0 }));
}

TEST(AliveActor, tryMoveToUnseen) {
	auto [actor, other] = createAliveActorTryMoveToTest();
	actor->tryMoveTo({ 1, 1, 0 });

	EXPECT_EQ(actor->position(), (sf::Vector3i{ 0, 1, 0 }));
}

TEST(AliveActor, tryMoveToSwap) {
	auto [actor, other] = createAliveActorTryMoveToTest();
	actor->tryMoveTo({ 0, 2, 0 });

	EXPECT_EQ(actor->position(), (sf::Vector3i{ 0, 2, 0 }));
	EXPECT_EQ(other->position(), (sf::Vector3i{ 0, 1, 0 }));
}