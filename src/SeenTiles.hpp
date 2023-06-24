/* This file is part of the Rune of the Eldest.
The Rune of the Eldest - Roguelike about the mage seeking for ancient knowledges
Copyright(C) 2023  PJutch

The Rune of the Eldest is free software : you can redistribute it and /or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

The Rune of the Eldest is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with the Rune of the Eldest.
If not, see < https://www.gnu.org/licenses/>. */

#ifndef SEEN_TILES_HPP_
#define SEEN_TILES_HPP_

class Player;
class World;

#include "Actor.hpp"

#include <SFML/System.hpp>

#include <vector>
#include <span>
#include <memory>

class SeenTiles {
public:
	SeenTiles(std::shared_ptr<Player> player, std::shared_ptr<World> world);

	enum class TileState {
		UNSEEN,
		MEMORIZED,
		VISIBLE,
	};

	TileState tileState(sf::Vector3i position) const noexcept {
		auto [x, y, z] = position;
		return tileStates[z][x * shapes[z].y + y];
	}

	[[nodiscard]] std::span<const std::unique_ptr<Actor::DrawMemento>> seenActors() const noexcept {
		return seenActors_;
	}

	void onGenerate();

	void update() {
		updateTiles();
		updateActors();
	}
private:
	std::vector< std::vector<TileState>> tileStates;
	std::vector<sf::Vector2i> shapes;

	std::vector<std::unique_ptr<Actor::DrawMemento>> seenActors_;

	std::shared_ptr<Player> player;
	std::shared_ptr<World> world;

	void updateTiles();
	void updateActors();

	TileState& tileStateMut(sf::Vector3i position) noexcept {
		auto [x, y, z] = position;
		return tileStates[z][x * shapes[z].y + y];
	}
};

#endif