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
	class TestActor : public core::Actor {
	public:
		TestActor() = default;
		TestActor(sf::Vector3i newPosition, int newId = -1) noexcept :
			position_{ newPosition }, id_{ newId } {}
		TestActor(double firstTurn, double turnDelay_, int newId = -1, std::vector<int>* log_ = nullptr,
			double waitAfter_ = std::numeric_limits<int>::max(),
			double dieAfter_ = std::numeric_limits<int>::max(), bool shouldInterruptOnDelete_ = false) noexcept :
			nextTurn_{ firstTurn }, turnDelay{ turnDelay_ }, id_{ newId }, log{ log_ },
			waitAfter{ waitAfter_ }, dieAfter{ dieAfter_ }, interruptOnDelete{ shouldInterruptOnDelete_ } {}

		sf::Vector3i position() const noexcept final {
			return position_;
		}

		void position(sf::Vector3i newPosition) noexcept final {
			position_ = newPosition;
		}

		bool act() override {
			if (log)
				log->push_back(id());
			nextTurn_ += turnDelay;
			return nextTurn_ <= waitAfter;
		}

		double nextTurn() const noexcept final {
			return nextTurn_;
		}

		bool isAlive() const noexcept final {
			return nextTurn_ <= dieAfter;
		}

		bool shouldInterruptOnDelete() const noexcept final {
			return interruptOnDelete;
		}

		void beDamaged(double damage) noexcept final {}

		AiState aiState() const noexcept final {
			return AiState::NONE;
		}

		const sf::Texture* texture() const final {
			return nullptr;
		}

		[[nodiscard]] double hp() const noexcept final {
			return 0;
		}

		[[nodiscard]] double maxHp() const noexcept final {
			return 0;
		}

		int id() const noexcept {
			return id_;
		}

		[[nodiscard]] bool isOnPlayerSide() const final {
			return false;
		}

		bool wantsSwap() const noexcept final {
			return true;
		}

		void handleSwap() noexcept final {}

		void handleSound(core::Sound sound) noexcept final {
			lastSound_ = sound;
		}

		core::Sound lastSound() const noexcept {
			return lastSound_;
		}
	private:
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
}

TEST(World, emptyActors) {
	core::World world;
	EXPECT_TRUE(world.actors().empty());
}

TEST(World, addActor) {
	auto actor1 = std::make_shared<TestActor>(sf::Vector3i{ 0, 0, 0 });
	auto actor2 = std::make_shared<TestActor>(sf::Vector3i{ 2, 3, 4 });

	core::World world;
	world.addActor(actor1);
	world.addActor(actor2);

	EXPECT_EQ(world.actors().size(), 2);
	EXPECT_EQ(std::ranges::count(world.actors(), actor1), 1);
	EXPECT_EQ(std::ranges::count(world.actors(), actor2), 1);
}

TEST(World, actorAt) {
	auto actor1 = std::make_shared<TestActor>(sf::Vector3i{ 0, 0, 0 });
	auto actor2 = std::make_shared<TestActor>(sf::Vector3i{ 2, 3, 4 });

	core::World world;
	world.addActor(actor1);
	world.addActor(actor2);

	EXPECT_EQ(world.actorAt({2, 3, 4}), actor2);
}

TEST(World, actorAtNull) {
	core::World world;
	world.addActor(std::make_shared<TestActor>(sf::Vector3i{ 0, 0, 0 }));
	world.addActor(std::make_shared<TestActor>(sf::Vector3i{ 2, 3, 4 }));

	EXPECT_EQ(world.actorAt({ 2, 3, 11 }), nullptr);
}

TEST(World, update) {
	std::vector<int> log;

	core::World world;
	world.addActor(std::make_shared<TestActor>(0, 2, 0,  &log));
	world.addActor(std::make_shared<TestActor>(7, 2, 1, &log, 7));

	world.update();

	ASSERT_EQ(log.size(), 5);
	EXPECT_EQ(log[0], 0);
	EXPECT_EQ(log[1], 0);
	EXPECT_EQ(log[2], 0);
	EXPECT_EQ(log[3], 0);
	EXPECT_EQ(log[4], 1);
}

TEST(World, updateMany) {
	std::vector<int> log;

	core::World world;
	world.addActor(std::make_shared<TestActor>(0, 2, 0, &log));
	world.addActor(std::make_shared<TestActor>(0, 2, 1, &log));
	world.addActor(std::make_shared<TestActor>(0, 2, 2, &log));
	world.addActor(std::make_shared<TestActor>(0, 2, 3, &log));
	world.addActor(std::make_shared<TestActor>(7, 2, 4, &log, 7));

	world.update();

	ASSERT_EQ(log.size(), 17);
	EXPECT_EQ(log[16], 4);
	EXPECT_EQ(std::ranges::count(log, 0), 4);
	EXPECT_EQ(std::ranges::count(log, 1), 4);
	EXPECT_EQ(std::ranges::count(log, 2), 4);
	EXPECT_EQ(std::ranges::count(log, 3), 4);
}

TEST(World, updateEmpty) {
	std::vector<int> log;
	core::World world;
	world.update();
	ASSERT_EQ(log.size(), 0);
}

TEST(World, updateDeath) {
	std::vector<int> log;

	core::World world;
	world.addActor(std::make_shared<TestActor>(0, 2, 0, &log));
	world.addActor(std::make_shared<TestActor>(0, 2, 1, &log, std::numeric_limits<int>::max(), 3));
	world.addActor(std::make_shared<TestActor>(7, 2, 4, &log, 7));

	world.update();

	ASSERT_EQ(log.size(), 7);
	EXPECT_EQ(log[6], 4);
	EXPECT_EQ(std::ranges::count(log, 0), 4);
	EXPECT_EQ(std::ranges::count(log, 1), 2);
}

TEST(World, updateDeathInterrupt) {
	std::vector<int> log;

	core::World world;
	world.addActor(std::make_shared<TestActor>(0, 2, 0, &log));
	world.addActor(std::make_shared<TestActor>(0, 2, 1, &log, std::numeric_limits<int>::max(), 3, true));

	world.update();

	EXPECT_EQ(std::ranges::count(log, 1), 2);
}

TEST(World, isFree) {
	core::World world;
	world.tiles().assign({ 2, 2, 1 }, Tile::WALL);

	world.tiles()[{ 0, 0, 0 }] = Tile::EMPTY;
	world.tiles()[{ 0, 1, 0 }] = Tile::EMPTY;

	world.addActor(std::make_shared<TestActor>(sf::Vector3i{0, 1, 0}));

	EXPECT_TRUE(world.isFree({ 0, 0, 0 }));
	EXPECT_FALSE(world.isFree({ 0, 1, 0 }));
	EXPECT_FALSE(world.isFree({ 1, 0, 0 }));
}

TEST(World, makeSound) {
	core::World world;
	auto actor1 = std::make_shared<TestActor>();
	auto actor2 = std::make_shared<TestActor>();
	world.addActor(actor1);
	world.addActor(actor2);

	core::Sound sound{ core::Sound::Type::ATTACK, true, {1, 0, 1} };
	world.makeSound(sound);

	EXPECT_EQ(actor1->lastSound(), sound);
	EXPECT_EQ(actor2->lastSound(), sound);
}
