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
#include "Controller/Controller.hpp"
#include "Effect/Effect.hpp"
#include "XpManager.hpp"
#include "DamageType.hpp"
#include "Spell/Spell.hpp"

#include "render/ParticleManager.hpp"
#include "render/coords.hpp"

#include "util/geometry.hpp"
#include "util/random.hpp"

#include <SFML/Graphics/Texture.hpp>

namespace core {
	/// Base for all Actors with hp and position in world
	class Actor : public std::enable_shared_from_this<Actor> {
	public:
		/// Immutable params of the Actor
		struct Stats {
			double maxHp;
			double regen;

			double maxMana = 0;
			double manaRegen = 0;

			double damage;
			double accuracy;

			double evasion;

			std::array<double, util::nEnumerators<DamageType>> defences;

			double turnDelay;

			double xp;

			const sf::Texture* texture;

			bool hasRangedAttack;
			const sf::Texture* projectileTexture;
			sf::Time projectileFlightTime;
		};

		Actor() = default;
		Actor(Stats stats, sf::Vector3i position, 
			  std::shared_ptr<World> world, std::shared_ptr<XpManager> xpManager, 
			  std::shared_ptr<render::ParticleManager> particles,
			  util::RandomEngine* randomEngine);
		Actor(Stats stats, 
			  std::shared_ptr<World> world, std::shared_ptr<XpManager> xpManager, 
			  std::shared_ptr<render::ParticleManager> particles,
			  util::RandomEngine* randomEngine);

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

		void beDamaged(double damage, DamageType type) {
			if (!isAlive()) 
				return;

			hp_ -= damage * recievedDamageMul(type);
			if (!isAlive())
				xpManager->addXp(stats.xp);
		}

		/// @brief Attacks this actor with given damage and accuracy
		/// @returns true if hit
		bool beAttacked(double damage, double accuracy, DamageType damageType) {
			if (isAlive() && std::bernoulli_distribution{hitChance(accuracy)}(randomEngine())) {
				beDamaged(damage, damageType);
				return true;
			} else {
				return false;
			}
		}

		/// Gets Actor HP
		[[nodiscard]] double hp() const noexcept {
			return hp_;
		}

		/// Gets max possible HP
		[[nodiscard]] double maxHp() const noexcept {
			return stats.maxHp * hpMul;
		}

		/// Gets Actor mana
		[[nodiscard]] double mana() const noexcept {
			return mana_;
		}

		/// Gets max possible mana
		[[nodiscard]] double maxMana() const noexcept {
			return stats.maxMana * manaMul;
		}

		/// @brief Tries to decreas Actor mana
		/// @param dmana amount of mana to use
		/// @returns true if Actor had enough mana
		bool useMana(double dmana) noexcept {
			if (mana_ < dmana)
				return false;

			mana_ -= dmana;
			return true;
		}

		[[nodiscard]] const sf::Texture* texture() const noexcept {
			return stats.texture;
		}

		void endTurn() noexcept;

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

		/// @brief Tries to move as near to given direction as possible
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		void tryRunAwayInDirection(sf::Vector2i direction, bool forceSwap);

		/// @brief Tries to move as much from player as possible
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		void tryRunAwayFromPlayer(bool forceSwap);

		[[nodiscard]] World& world() noexcept {
			return *world_;
		}

		[[nodiscard]] const World& world() const noexcept {
			return *world_;
		}

		[[nodiscard]] util::RandomEngine& randomEngine() noexcept {
			return *randomEngine_;
		}

		/// @brief Checks if can move to newPosition or swap with Actor there
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		[[nodiscard]] bool canMoveToOrAttack(sf::Vector3i newPosition, bool forceSwap) const;

		/// @brief Checks if can move to position + offset or swap with Actor there
		/// @param forceSwap Forces swap even if other Actor doesn't want it
		[[nodiscard]] bool canMoveToOrAttack(sf::Vector2i offset, bool forceSwap) const {
			return canMoveToOrAttack(position() + util::make3D(offset, 0), forceSwap);
		}

		void addEffect(std::unique_ptr<Effect> effect) {
			effect->owner(weak_from_this());
			effects_.push_back(std::move(effect));
			updateHp();
		}

		const std::vector<std::unique_ptr<Effect>>& effects() const noexcept {
			return effects_;
		}

		void addSpell(std::shared_ptr<Spell> spell) {
			spells_.push_back(std::move(spell));
		}

		const std::vector<std::shared_ptr<Spell>>& spells() const noexcept {
			return spells_;
		}

		double xpMul() const;

		void updateHp();

		bool hasRangedAttack() {
			return stats.hasRangedAttack;
		}

		void attack(Actor& other);

		void rangedAttack(Actor& other) {
			attack(other);

			auto pos1 = render::toScreen(util::geometry_cast<float>(util::getXY(position())) + sf::Vector2f{0.5f, 0.5f});
			auto pos2 = render::toScreen(util::geometry_cast<float>(util::getXY(other.position())) + sf::Vector2f{0.5f, 0.5f});
			particles->add(pos1, pos2, position().z,
				           stats.projectileFlightTime, stats.projectileTexture);
		}

		double hitChance(double accuracy) {
			return accuracy / (accuracy + evasion());
		}

		void heal(double healed) {
			hp_ += healed;
			hp_ = std::min(hp(), maxHp());
		}
	private:
		Stats stats;
		std::unique_ptr<Controller> controller_;
		std::vector<std::unique_ptr<Effect>> effects_;
		std::vector<std::shared_ptr<Spell>> spells_;

		double nextTurn_ = 0;
		sf::Vector3i position_;
		double hp_;
		double hpMul = 1;

		double mana_;
		double manaMul = 1;

		std::shared_ptr<World> world_;
		std::shared_ptr<XpManager> xpManager;
		std::shared_ptr<render::ParticleManager> particles;
		util::RandomEngine* randomEngine_;

		double regen();
		double manaRegen();
		double damage(const Actor& target);
		double accuracy();
		double evasion();
		double turnDelay();
		double defence(DamageType damageType);
		double recievedDamageMul(DamageType damageType);
	};
}

#endif