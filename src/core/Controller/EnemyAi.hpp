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

#include "../fwd.hpp"
#include "../AiState.hpp"
#include "../Sound.hpp"
#include "Controller.hpp"

#include "render/AssetManager.hpp"

#include "util/random.hpp"
#include "util/raycast.hpp"
#include "util/pathfinding.hpp"
#include "util/stringify.hpp"

#include <SFML/System/Vector3.hpp>

#include <array>
#include <memory>

namespace core {
	/// AI controlling enemy
	class EnemyAi : public Controller {
	public:
		struct State {
			sf::Vector3i targetPosition;
			double targetPriority = 0.0;
			bool checkStairs = false;
			bool wandering = false;
		};

		EnemyAi(State state, std::weak_ptr<Actor> enemy, std::shared_ptr<util::Raycaster> raycaster);
		EnemyAi(std::weak_ptr<Actor> enemy, std::shared_ptr<util::Raycaster> raycaster);

		/// Chases or attacks Player
		bool act() final;

		void handleSwap() noexcept final {
			wantsSwap(false);
		}

		void handleSound(Sound sound) noexcept final;

		[[nodiscard]] AiState aiState() const noexcept final;

		[[nodiscard]] virtual std::string stringify() const {
			return std::format("enemy targetPosition {}, targetPriority {}, checkStairs {}, wandering {}", 
				util::stringifyVector3(state.targetPosition), state.targetPriority, state.checkStairs, state.wandering);
		}
	private:
		std::weak_ptr<Actor> enemy_;

		State state;

		std::shared_ptr<util::Raycaster> raycaster;
		std::unique_ptr<util::PathBuffer> pathBuffer;

		bool canSeePlayer() const noexcept;

		void updateTarget() noexcept;

		void setTarget(sf::Vector3i position, double priority) {
			state.targetPosition = position;
			state.targetPriority = priority;
			state.checkStairs = true;
			state.wandering = false;
		}

		void wander() noexcept {
			state.targetPosition = randomNearbyTarget();
			state.wandering = true;
			state.checkStairs = false;
		}

		sf::Vector3i randomNearbyTarget() noexcept;
		sf::Vector3i tryFollowStairs(sf::Vector3i position) noexcept;

		void travelToTarget() noexcept;
	};
}

#endif