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

#include "util/random.hpp"

#include <SFML/System.hpp>

#include <array>
#include <memory>

namespace core {
	/// Goblin enemy
	class Goblin : public AliveActor {
	public:
		Goblin(sf::Vector3i newPosition, std::shared_ptr<World> world_, std::shared_ptr<Player> player_, util::RandomEngine& randomEngine_) :
			AliveActor{ 3, 0.1, newPosition, std::move(world_), &randomEngine_ }, player{ player_ }, targetPosition{ newPosition } {}

		/// Randomly moves goblin
		bool act() final;

		static void spawnSingle(int level, std::shared_ptr<World> world, std::shared_ptr<Player> player_, util::RandomEngine& randomEngine);
		static void spawnAll(std::shared_ptr<World> world, std::shared_ptr<Player> player_, util::RandomEngine& randomEngine);

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
		AiState aiState() const noexcept {
			return aiState_;
		}

		void handleSound(Sound sound) noexcept final;

		class DrawMemento : public AliveActor::DrawMemento {
		public:
			DrawMemento(const Goblin& goblin) : AliveActor::DrawMemento{ goblin }, aiState_{ goblin.aiState() } {}

			/// Gets the saved AI state
			AiState aiState() const noexcept {
				return aiState_;
			}

			void draw(render::Renderer& renderer) const final;
		private:
			AiState aiState_;
		};

		[[nodiscard]] std::unique_ptr<Actor::DrawMemento> createDrawMemento() const final {
			return std::make_unique<DrawMemento>(*this);
		}
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