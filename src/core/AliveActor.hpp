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

#include "Actor.hpp"

#include "World.hpp"

#include "util/geometry.hpp"

namespace core {
	/// Base for all Actors with hp and position in world
	class AliveActor : public Actor {
	public:
		[[nodiscard]] sf::Vector3i position() const noexcept final {
			return position_;
		}

		void position(sf::Vector3i newPosition) noexcept final {
			position_ = newPosition;
		}

		[[nodiscard]] int nextTurn() const noexcept final {
			return nextTurn_;
		}

		[[nodiscard]] bool isAlive() const final {
			return hp() > 0;
		}

		void beDamaged(double damage) final {
			hp_ -= damage;
		}

		/// Gets Actor HP
		[[nodiscard]] double hp() const noexcept {
			return hp_;
		}

		/// Gets max possible HP
		[[nodiscard]] double maxHp() const noexcept {
			return maxHp_;
		}
	protected:
		AliveActor(double newMaxHp, double regen_, sf::Vector3i newPosition, std::shared_ptr<World> newWorld, util::RandomEngine* newRandomEngine);
		AliveActor(double newMaxHp, double regen_, std::shared_ptr<World> newWorld, util::RandomEngine* newRandomEngine);

		[[nodiscard]] World& world() noexcept {
			return *world_;
		}

		[[nodiscard]] const World& world() const noexcept {
			return *world_;
		}

		[[nodiscard]] util::RandomEngine& randomEngine() noexcept {
			return *randomEngine_;
		}

		/// Sets next turn time
		void nextTurn(int newNextTurn) noexcept {
			nextTurn_ = newNextTurn;
		}

		/// Dealys next Actor turn and applies over time effects
		void wait(int time) noexcept;

		/// Attacks other Actor
		virtual void attack(Actor& actor) = 0;

		/// Callled after successful move
		virtual void moveSucceed() {}

		/// @brief Checks if can move to newPosition or swap with Actor there
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		[[nodiscard]] bool canMoveToOrAttack(sf::Vector3i newPosition, bool forceSwap) const;

		/// @brief Checks if can move to position + offset or swap with Actor there
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		[[nodiscard]] bool canMoveToOrAttack(sf::Vector2i offset, bool forceSwap) const {
			return canMoveToOrAttack(position() + util::make3D(offset, 0), forceSwap);
		}

		/// @brief Changes position if newPosition isn't occupied
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		void tryMoveTo(sf::Vector3i newPosition, bool forceSwap);

		/// @brief Changes position if newPosition isn't occupied
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		void tryMoveTo(sf::Vector2i newPosition, bool forceSwap) {
			tryMoveTo(util::make3D(newPosition, position().z), forceSwap);
		}

		/// @brief Changes position if position + offset isn't occupied
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		void tryMove(sf::Vector3i offset, bool forceSwap) {
			tryMoveTo(position() + offset, forceSwap);
		}

		/// @brief Changes position if position + offset isn't occupied
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		void tryMove(sf::Vector2i offset, bool forceSwap) {
			tryMoveTo(util::getXY(position()) + offset, forceSwap);
		}

		/// @brief Tries to move in given direction or 45 degrees left or right
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		void tryMoveInDirection(sf::Vector2i direction, bool forceSwap);

		void hp(double newHp) noexcept {
			hp_ = newHp;
		}

		class DrawMemento : public Actor::DrawMemento {
		public:
			DrawMemento(const AliveActor& actor) :
				position_{ actor.position() }, hp_{ actor.hp() }, maxHp_{ actor.maxHp() } {}

			/// Gets the saved position
			[[nodiscard]] sf::Vector3i position() const noexcept {
				return position_;
			}

			/// Gets the saved HP
			[[nodiscard]] double hp() const noexcept {
				return hp_;
			}

			/// Gets the saved max HP
			[[nodiscard]] double maxHp() const noexcept {
				return maxHp_;
			}
		private:
			sf::Vector3i position_;
			double hp_;
			double maxHp_;
		};
	private:
		int nextTurn_ = 0;
		sf::Vector3i position_;

		double hp_;
		double maxHp_;
		double regen;

		std::shared_ptr<World> world_;
		util::RandomEngine* randomEngine_;
	};
}

#endif