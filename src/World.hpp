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
#include "Player.hpp"
#include "Actor.hpp"

#include <queue>

/// Dungeons and all objects in it
class World : public std::enable_shared_from_this<World> {
public:
	World(std::shared_ptr<Dungeon> dungeon, std::shared_ptr<Player> player, RandomEngine& randomEngine, LoggerFactory& loggerFactory);

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

	void player(std::shared_ptr<Player> newPlayer) {
		player_ = std::move(newPlayer);
		actors_.push_back(player_);
		push_actor();
	}

	/// @brief Generates dungeon and places player
	/// @param logger logger to log messages
	void generate();

	void update() {
		while (!actors_.empty() && actors_.front()->act()) {
			push_actor();
			pop_actor();
		}
	}

	bool isPassable(sf::Vector3i position) {
		return isPassableTile(dungeon().at(position)) 
			&& std::ranges::find(actors_, position, &Actor::position3) == actors_.end();
	}

	std::span<const std::shared_ptr<Actor>> actors() const {
		return actors_;
	}
private:
	std::shared_ptr<Dungeon> dungeon_;
	std::shared_ptr<Player> player_;

	std::vector<std::shared_ptr<Actor>> actors_;

	void push_actor() {
		std::ranges::push_heap(actors_, std::greater<>{}, [](std::shared_ptr<Actor>& actor) {
			return actor->nextTurn();
		});
	}

	void pop_actor() {
		std::ranges::pop_heap(actors_, std::greater<>{}, [](std::shared_ptr<Actor>& actor) {
			return actor->nextTurn();
		});
	}

	std::shared_ptr<spdlog::logger> generationLogger;
	RandomEngine* randomEngine;

	void spawnGoblins(int level);
};

#endif