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

#include "core/World.hpp"
#include "core/Actor.hpp"

#include <gtest/gtest.h>

namespace {
	class TestController : public core::Controller {
	public:
		TestController(std::weak_ptr<core::Actor> actor_, int newId = -1) : 
			actor{ std::move(actor_) }, id_{ newId } {}
		TestController(std::weak_ptr<core::Actor> actor_, int newId, std::vector<int>* log_,
			           double waitAfter_ = std::numeric_limits<double>::infinity(),
			           double dieAfter_ = std::numeric_limits<double>::infinity(), bool shouldInterruptOnDelete_ = false) noexcept :
				actor{ std::move(actor_) }, id_{ newId }, log{ log_ }, waitAfter{ waitAfter_ }, dieAfter{ dieAfter_ } {
			shouldInterruptOnDelete(shouldInterruptOnDelete_);
		}

		bool act() final {
			auto actor_ = actor.lock();

			if (log)
				log->push_back(id());

			actor_->endTurn();

			if (actor_->nextTurn() > dieAfter)
				actor_->beDamaged(std::numeric_limits<double>::infinity());

			return actor_->nextTurn() <= waitAfter;
		}

		int id() const noexcept {
			return id_;
		}

		void handleSwap() noexcept final {}

		void handleSound(core::Sound sound) noexcept final {
			lastSound_ = sound;
		}

		core::Sound lastSound() const noexcept {
			return lastSound_;
		}
	private:
		std::weak_ptr<core::Actor> actor;

		double nextTurn_ = 0;
		double turnDelay = 0;
		sf::Vector3i position_{ 0, 0, 0 };

		int id_ = -1;

		std::vector<int>* log = nullptr;

		double waitAfter = std::numeric_limits<int>::max();
		double dieAfter = std::numeric_limits<int>::max();
		bool interruptOnDelete = false;

		core::Sound lastSound_{};
	};

	auto testXpManager = std::make_shared<core::XpManager>();

	std::shared_ptr<core::Actor> makeTestActor() {
		return std::make_shared<core::Actor>(core::Actor::Stats{ .maxHp = 1 }, nullptr, testXpManager, nullptr);
	}

	std::shared_ptr<core::Actor> makeTestActor(sf::Vector3i position) {
		return std::make_shared<core::Actor>(core::Actor::Stats{ .maxHp = 1 }, position, nullptr, testXpManager, nullptr);
	}

	std::shared_ptr<core::Actor> makeTestActor(double turnDelay) {
		return std::make_shared<core::Actor>(core::Actor::Stats{ .maxHp = 1, .turnDelay = turnDelay }, nullptr, testXpManager, nullptr);
	}

	std::shared_ptr<core::Actor> makeTestActor(double turnDelay, std::shared_ptr<core::XpManager> xpManager) {
		return std::make_shared<core::Actor>(core::Actor::Stats{.maxHp = 1, .turnDelay = turnDelay}, 
			                                 nullptr, std::move(xpManager), nullptr);
	}
}

TEST(World, emptyActors) {
	core::World world;
	EXPECT_TRUE(world.actors().empty());
}

TEST(World, addActor) {
	auto actor1 = makeTestActor({ 0, 0, 0 });
	auto actor2 = makeTestActor({ 2, 3, 4 });

	core::World world;
	world.addActor(actor1);
	world.addActor(actor2);

	EXPECT_EQ(world.actors().size(), 2);
	EXPECT_EQ(std::ranges::count(world.actors(), actor1), 1);
	EXPECT_EQ(std::ranges::count(world.actors(), actor2), 1);
}

TEST(World, actorAt) {
	auto actor1 = makeTestActor({ 0, 0, 0 });
	auto actor2 = makeTestActor({ 2, 3, 4 });

	core::World world;
	world.addActor(actor1);
	world.addActor(actor2);

	EXPECT_EQ(world.actorAt({2, 3, 4}), actor2);
}

TEST(World, actorAtNull) {
	core::World world;
	world.addActor(makeTestActor({ 0, 0, 0 }));
	world.addActor(makeTestActor({ 2, 3, 4 }));

	EXPECT_EQ(world.actorAt({ 2, 3, 11 }), nullptr);
}

TEST(World, update) {
	core::World world;
	std::vector<int> log;

	auto actor1 = makeTestActor(2);
	actor1->controller(std::make_unique<TestController>(actor1, 0, &log));
	world.addActor(std::move(actor1));

	auto actor2 = makeTestActor(7);
	actor2->controller(std::make_unique<TestController>(actor2, 1, &log, 7));
	world.addActor(std::move(actor2));

	world.update();

	ASSERT_EQ(log.size(), 6);
	EXPECT_EQ(std::ranges::count(log, 0), 4);
	EXPECT_EQ(std::ranges::count(log, 1), 2);
	EXPECT_EQ(log[5], 1);
}

TEST(World, updateMany) {
	std::vector<int> log;
	core::World world;

	auto actor1 = makeTestActor(2);
	actor1->controller(std::make_unique<TestController>(actor1, 0, &log));
	world.addActor(std::move(actor1));

	auto actor2 = makeTestActor(2);
	actor2->controller(std::make_unique<TestController>(actor2, 1, &log));
	world.addActor(std::move(actor2));

	auto actor3 = makeTestActor(2);
	actor3->controller(std::make_unique<TestController>(actor3, 2, &log));
	world.addActor(std::move(actor3));

	auto actor4 = makeTestActor(2);
	actor4->controller(std::make_unique<TestController>(actor4, 3, &log));
	world.addActor(std::move(actor4));

	auto actor5 = makeTestActor(7);
	actor5->controller(std::make_unique<TestController>(actor5, 4, &log, 7));
	world.addActor(std::move(actor5));

	world.update();

	ASSERT_EQ(log.size(), 18);
	EXPECT_EQ(std::ranges::count(log, 0), 4);
	EXPECT_EQ(std::ranges::count(log, 1), 4);
	EXPECT_EQ(std::ranges::count(log, 2), 4);
	EXPECT_EQ(std::ranges::count(log, 3), 4);
	EXPECT_EQ(std::ranges::count(log, 4), 2);
	EXPECT_EQ(log[17], 4);
}

TEST(World, updateDeath) {
	std::vector<int> log;
	auto world = std::make_shared<core::World>();
	auto xpManager = std::make_shared<core::XpManager>(world);

	auto actor1 = makeTestActor(2, xpManager);
	actor1->controller(std::make_unique<TestController>(actor1, 0, &log));
	world->addActor(actor1);

	auto actor2 = makeTestActor(2, xpManager);
	actor2->controller(std::make_unique<TestController>(actor2, 1, &log, std::numeric_limits<double>::infinity(), 3));
	world->addActor(std::move(actor2));

	auto actor3 = makeTestActor(7, xpManager);
	actor3->controller(std::make_unique<TestController>(actor3, 2, &log, 7));
	world->addActor(std::move(actor3));

	world->player(actor1);

	world->update();

	ASSERT_EQ(log.size(), 8);
	EXPECT_EQ(std::ranges::count(log, 0), 4);
	EXPECT_EQ(std::ranges::count(log.begin(), log.begin() + 5, 1), 2);
	EXPECT_EQ(std::ranges::count(log.begin() + 5, log.end(), 1), 0);
	EXPECT_EQ(std::ranges::count(log, 2), 2);
	EXPECT_EQ(log[7], 2);
}

TEST(World, updateDeathInterrupt) {
	std::vector<int> log;
	auto world = std::make_shared<core::World>();
	auto xpManager = std::make_shared<core::XpManager>(world);

	auto actor1 = makeTestActor(2, xpManager);
	actor1->controller(std::make_unique<TestController>(actor1, 0, &log));
	world->addActor(actor1);

	auto actor2 = makeTestActor(2, xpManager);
	actor2->controller(std::make_unique<TestController>(actor2, 1, &log, std::numeric_limits<double>::infinity(), 3, true));
	world->addActor(std::move(actor2));

	world->player(actor1);

	world->update();

	EXPECT_GE(std::ranges::count(log, 0), 1);
	EXPECT_EQ(std::ranges::count(log, 1), 2);
}

TEST(World, isFree) {
	core::World world;
	world.tiles().assign({ 2, 2, 1 }, Tile::WALL);

	world.tiles()[{ 0, 0, 0 }] = Tile::EMPTY;
	world.tiles()[{ 0, 1, 0 }] = Tile::EMPTY;

	world.addActor(makeTestActor({0, 1, 0}));

	EXPECT_TRUE(world.isFree({ 0, 0, 0 }));
	EXPECT_FALSE(world.isFree({ 0, 1, 0 }));
	EXPECT_FALSE(world.isFree({ 1, 0, 0 }));
}

TEST(World, makeSound) {
	core::World world;

	auto actor1 = makeTestActor();
	actor1->controller(std::make_unique<TestController>(actor1));
	world.addActor(actor1);

	auto actor2 = makeTestActor();
	actor2->controller(std::make_unique<TestController>(actor2));
	world.addActor(actor2);

	core::Sound sound{ core::Sound::Type::ATTACK, true, {1, 0, 1} };
	world.makeSound(sound);

	EXPECT_EQ(dynamic_cast<TestController&>(actor1->controller()).lastSound(), sound);
	EXPECT_EQ(dynamic_cast<TestController&>(actor2->controller()).lastSound(), sound);
}
