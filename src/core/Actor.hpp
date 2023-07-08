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

#ifndef ALIVE_ACTOR_HPP_
#define ALIVE_ACTOR_HPP_

#include "World.hpp"
#include "Controller.hpp"

#include "util/geometry.hpp"
#include "util/random.hpp"

#include <SFML/Graphics/Texture.hpp>

namespace core {
	/// Base for all Actors with hp and position in world
	class Actor {
	public:
		/// Immutable params of the Actor
		struct Stats {
			double maxHp;
			double regen;
			double damage;
			double turnDelay;
			const sf::Texture* texture;
		};

		Actor() = default;
		Actor(Stats stats, sf::Vector3i newPosition, std::shared_ptr<World> newWorld, util::RandomEngine* newRandomEngine);
		Actor(Stats stats, std::shared_ptr<World> newWorld, util::RandomEngine* newRandomEngine);

		void controller(std::unique_ptr<Controller> newController) {
			controller_ = std::move(newController);
		}

		Controller& controller() noexcept {
			return *controller_;
		}

		const Controller& controller() const noexcept {
			return *controller_;
		}

		[[nodiscard]] sf::Vector3i position() const noexcept {
			return position_;
		}

		void position(sf::Vector3i newPosition) noexcept {
			position_ = newPosition;
		}

		[[nodiscard]] double nextTurn() const noexcept {
			return nextTurn_;
		}

		[[nodiscard]] bool isAlive() const {
			return hp() > 0;
		}

		void beDamaged(double damage) {
			hp_ -= damage;
		}

		/// Gets Actor HP
		[[nodiscard]] double hp() const noexcept {
			return hp_;
		}

		/// Gets max possible HP
		[[nodiscard]] double maxHp() const noexcept {
			return stats.maxHp;
		}

		[[nodiscard]] const sf::Texture* texture() const noexcept {
			return stats.texture;
		}

		void endTurn() noexcept {
			wait(stats.turnDelay);
		}

		/// @brief Changes position if newPosition isn't occupied or attacks Actor there
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		/// @returns true if moved or attacked
		bool tryMoveTo(sf::Vector3i newPosition, bool forceSwap);

		/// @brief Changes position if newPosition isn't occupied or attacks Actor there
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		/// @returns true if moved or attacked
		bool tryMoveTo(sf::Vector2i newPosition, bool forceSwap) {
			return tryMoveTo(util::make3D(newPosition, position().z), forceSwap);
		}

		/// @brief Changes position if position + offset isn't occupied or attacks Actor there
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		/// @returns true if moved or attacked
		bool tryMove(sf::Vector3i offset, bool forceSwap) {
			return tryMoveTo(position() + offset, forceSwap);
		}

		/// @brief Changes position if position + offset isn't occupied or attacks Actor there
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		/// @returns true if moved or attacked
		bool tryMove(sf::Vector2i offset, bool forceSwap) {
			return tryMoveTo(util::getXY(position()) + offset, forceSwap);
		}

		/// @brief Tries to move in given direction or 45 degrees left or right
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		void tryMoveInDirection(sf::Vector2i direction, bool forceSwap);

		[[nodiscard]] World& world() noexcept {
			return *world_;
		}

		[[nodiscard]] const World& world() const noexcept {
			return *world_;
		}

		[[nodiscard]] util::RandomEngine& randomEngine() noexcept {
			return *randomEngine_;
		}

		bool act() {
			return controller_->act();
		}

		[[nodiscard]] bool shouldInterruptOnDelete() const {
			return controller_->shouldInterruptOnDelete();
		}

		[[nodiscard]] bool isOnPlayerSide() const {
			return controller_->isOnPlayerSide();
		}

		[[nodiscard]] bool wantsSwap() const noexcept {
			return controller_->wantsSwap();
		}

		void handleSwap() noexcept {
			controller_->handleSwap();
		}

		void handleSound(Sound sound) noexcept {
			controller_->handleSound(sound);
		}

		AiState aiState() const noexcept {
			return controller_->aiState();
		}
	protected:
		/// Sets next turn time
		void nextTurn(double newNextTurn) noexcept {
			nextTurn_ = newNextTurn;
		}

		/// Dealys next Actor turn and applies over time effects
		void wait(double time) noexcept;

		/// @brief Checks if can move to newPosition or swap with Actor there
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		[[nodiscard]] bool canMoveToOrAttack(sf::Vector3i newPosition, bool forceSwap) const;

		/// @brief Checks if can move to position + offset or swap with Actor there
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		[[nodiscard]] bool canMoveToOrAttack(sf::Vector2i offset, bool forceSwap) const {
			return canMoveToOrAttack(position() + util::make3D(offset, 0), forceSwap);
		}

		void hp(double newHp) noexcept {
			hp_ = newHp;
		}
	private:
		Stats stats;
		std::unique_ptr<Controller> controller_;

		double nextTurn_ = 0;
		sf::Vector3i position_;
		double hp_;

		std::shared_ptr<World> world_;
		util::RandomEngine* randomEngine_;
	};
}

#endif