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

#ifndef ENEMY_HPP_
#define ENEMY_HPP_

#include "AliveActor.hpp"

#include "fwd.hpp"
#include "AiState.hpp"

#include "render/AssetManager.hpp"

#include "util/random.hpp"

#include <SFML/System/Vector3.hpp>

#include <array>
#include <memory>

namespace core {
	/// Customizable enemy
	class Enemy : public AliveActor {
	public:
		Enemy(sf::Vector3i newPosition, const sf::Texture& texture, std::shared_ptr<World> world_, 
			  std::shared_ptr<Player> player_, util::RandomEngine& randomEngine_) :
			AliveActor{ 3, 0.1, texture, newPosition, std::move(world_), &randomEngine_ },
			player{ player_ }, targetPosition{ newPosition } {}

		/// Randomly moves goblin
		bool act() final;

		static void spawnSingle(int level, const sf::Texture& texture, std::shared_ptr<World> world,
			                    std::shared_ptr<Player> player, util::RandomEngine& randomEngine) {
			sf::Vector3i position = world->randomPositionAt(level, &World::isFree);
			world->addActor(std::make_shared<Enemy>(position, texture, world, std::move(player), randomEngine));
		}

		static void spawnAll(std::shared_ptr<World> world, std::shared_ptr<Player> player, 
			                 std::shared_ptr<render::AssetManager> assets, util::RandomEngine& randomEngine);

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
		AiState aiState() const noexcept final {
			return aiState_;
		}

		void handleSound(Sound sound) noexcept final;
	private:
		std::shared_ptr<Player> player;
		bool wantsSwap_ = true;

		sf::Vector3i targetPosition;
		double targetPriority = 0.01;
		AiState aiState_ = AiState::INACTIVE;

		void attack(Actor& actor) final {
			actor.beDamaged(1);
			world().makeSound({ Sound::Type::ATTACK, false, position() });
		}

		bool canSeePlayer() const noexcept;

		void updateTarget() noexcept;
		sf::Vector3i randomNearbyTarget() noexcept;
		sf::Vector3i tryFollowStairs(sf::Vector3i position) noexcept;

		void travelToTarget() noexcept;

		void moveSucceed() final {
			world().makeSound({ Sound::Type::WALK, false, position() });
		}
	};
}

#endif