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

#ifndef GOBLIN_HPP_
#define GOBLIN_HPP_

#include "AliveActor.hpp"

#include "Player.hpp"
#include "AiState.hpp"

#include "Renderer.hpp"

#include "random.hpp"
#include "geometry.hpp"

#include <SFML/System.hpp>

#include <array>
#include <memory>

/// Goblin enemy
class Goblin : public AliveActor {
public:
	Goblin(sf::Vector3i newPosition, std::shared_ptr<World> world_, std::shared_ptr<Player> player_, RandomEngine& randomEngine_) :
		AliveActor{ 3, 0.1, newPosition, std::move(world_), &randomEngine_ }, player{ player_ }, targetPosition{ newPosition } {}

	/// Randomly moves goblin
	bool act() final;

	static void spawnSingle(int level, std::shared_ptr<World> world, std::shared_ptr<Player> player_, RandomEngine& randomEngine);
	static void spawnAll(std::shared_ptr<World> world, std::shared_ptr<Player> player_, RandomEngine& randomEngine);

	void draw(Renderer& renderer) const final {
		if (isAlive())
			renderer.draw(*this);
	}

	[[nodiscard]] bool shouldInterruptOnDelete() const final {
		return false;
	}

	[[nodiscard]] bool isOnPlayerSide() const final {
		return false;
	}

	[[nodiscard]] bool wantsSwap() const noexcept final {
		return wantsSwap_;
	}

	void handleSwap() noexcept final {
		wantsSwap_ = false;
	}

	/// Gets Goblin's AI state
	AiState aiState() const noexcept;
private:
	std::shared_ptr<Player> player;
	bool wantsSwap_ = true;

	sf::Vector3i targetPosition;

	void attack(Actor& actor) final {
		actor.beDamaged(1);
	}

	bool canSeePlayer() const noexcept {
		return position().z == player->position().z
			&& uniformDistance(position(), player->position()) <= 7;
	}
};

#endif