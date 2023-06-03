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

#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "Dungeon.hpp"
#include "Actor.hpp"

#include <queue>

/// Dungeons and all objects in it
class World : public std::enable_shared_from_this<World> {
public:
	World(std::shared_ptr<Dungeon> newDungeon) :
		dungeon_{ std::move(newDungeon) } {}

	DungeonGenerator& dungeonGenerator() noexcept {
		return dungeon().dungeonGenerator();
	}

	const DungeonGenerator& dungeonGenerator() const noexcept {
		return dungeon().dungeonGenerator();
	}

	Dungeon& dungeon() noexcept {
		return *dungeon_;
	}

	const Dungeon& dungeon() const noexcept {
		return *dungeon_;
	}

	void addActor(std::shared_ptr<Actor> actor) {
		actors_.push_back(std::move(actor));
		pushActor();
	}

	void clearActors() {
		actors_.clear();
	}

	std::span<const std::shared_ptr<Actor>> actors() const {
		return actors_;
	}

	/// @brief Gets Actor at given position if it exist
	/// @warning May return nullptr
	std::shared_ptr<Actor> actorAt(sf::Vector3i position) {
		auto iter = std::ranges::find_if(actors_, [position](std::shared_ptr<Actor> actor) {
			return actor->isAlive() && actor->position() == position;
		});

		if (iter == actors_.end())
			return nullptr;
		return *iter;
	}

	/// @brief Generates dungeon and places player
	/// @param logger logger to log messages
	void generate(std::shared_ptr<spdlog::logger> logger);

	void update() {
		while (!actors_.empty()) {
			popActor();

			if (actors_.back()->isAlive()) {
				bool complete = actors_.back()->act();
				pushActor();
				if (!complete)
					break;
			} else {
				bool interrupt = actors_.back()->shouldInterruptOnDelete();
				actors_.pop_back();
				if (interrupt)
					break;
			}
		}
	}

	/// Tile isPassable and have no Actors on it
	bool isFree(sf::Vector3i position) {
		return isPassable(dungeon().at(position)) && !actorAt(position);
	}
private:
	std::shared_ptr<Dungeon> dungeon_;
	std::vector<std::shared_ptr<Actor>> actors_;

	void pushActor() {
		std::ranges::push_heap(actors_, std::greater<>{}, [](std::shared_ptr<Actor>& actor) {
			return actor->nextTurn();
		});
	}

	void popActor() {
		std::ranges::pop_heap(actors_, std::greater<>{}, [](std::shared_ptr<Actor>& actor) {
			return actor->nextTurn();
		});
	}

	void spawnGoblins(int level);
};

#endif