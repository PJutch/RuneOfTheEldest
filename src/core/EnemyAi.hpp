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

#ifndef ENEMY_AI_HPP_
#define ENEMY_AI_HPP_

#include "fwd.hpp"
#include "AiState.hpp"
#include "Sound.hpp"
#include "Controller.hpp"

#include "render/AssetManager.hpp"

#include "util/random.hpp"

#include <SFML/System/Vector3.hpp>

#include <array>
#include <memory>

namespace core {
	/// AI controlling enemy
	class EnemyAi : public Controller {
	public:
		EnemyAi(std::weak_ptr<AliveActor> newEnemy_);

		bool act() final;

		void handleEvent(sf::Event event) noexcept final {}

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

		AiState aiState() const noexcept final {
			return state_;
		}

		void handleSound(Sound sound) noexcept final;
	private:
		std::weak_ptr<AliveActor> enemy_;

		bool wantsSwap_ = true;
		sf::Vector3i targetPosition;
		double targetPriority = 0.01;
		AiState state_ = AiState::INACTIVE;

		bool canSeePlayer() const noexcept;

		void updateTarget() noexcept;
		sf::Vector3i randomNearbyTarget() noexcept;
		sf::Vector3i tryFollowStairs(sf::Vector3i position) noexcept;

		void travelToTarget() noexcept;
	};
}

#endif