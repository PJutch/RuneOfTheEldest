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

#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "AliveActor.hpp"
#include "PlayerController.hpp"

#include "World.hpp"

#include "render/AssetManager.hpp"

#include "util/geometry.hpp"

#include <SFML/Window/Event.hpp>

namespace core {
	/// Player character
	class Player : public AliveActor, public std::enable_shared_from_this<Player> {
	public:
		Player(std::shared_ptr<World> world_, std::shared_ptr<render::AssetManager> assets, util::RandomEngine& randomEngine_) :
			AliveActor{ {.maxHp = 10, .regen = 0.1, .damage = 2, .turnDelay = 1, .texture = &assets->playerTexture()},
			            std::move(world_), &randomEngine_ } {}
		Player() = default;

		void spawn();
	};
}

#endif