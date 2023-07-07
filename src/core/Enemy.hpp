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
#include "EnemyAI.hpp"

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
		using AliveActor::AliveActor;

		void ai(std::unique_ptr<EnemyAi> newAi) {
			ai_ = std::move(newAi);
		}

		bool act() final {
			return ai_->act();
		}

		[[nodiscard]] bool shouldInterruptOnDelete() const final {
			return ai_->shouldInterruptOnDelete();
		}

		[[nodiscard]] bool isOnPlayerSide() const final {
			return ai_->isOnPlayerSide();
		}

		[[nodiscard]] bool wantsSwap() const noexcept final {
			return ai_->wantsSwap();
		}

		void handleSwap() noexcept final {
			ai_->handleSwap();
		}

		/// Gets Goblin's AI state
		AiState aiState() const noexcept final {
			return ai_->state();
		}

		void handleSound(Sound sound) noexcept final {
			ai_->handleSound(sound);
		}

		const World& world() const noexcept {
			return AliveActor::world();
		}

		util::RandomEngine& randomEngine() noexcept {
			return AliveActor::randomEngine();
		}

		bool tryMove(sf::Vector3i offset, bool forceSwap) {
			return AliveActor::tryMove(offset, forceSwap);
		}

		void tryMoveInDirection(sf::Vector2i direction, bool forceSwap) {
			AliveActor::tryMoveInDirection(direction, forceSwap);
		}

		void endTurn() {
			AliveActor::endTurn();
		}
	private:
		std::unique_ptr<EnemyAi> ai_;

		std::shared_ptr<Player> player;
	};
}

#endif