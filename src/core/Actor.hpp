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

#ifndef ACTOR_HPP_
#define ACTOR_HPP_

#include "World.hpp"
#include "Controller/Controller.hpp"
#include "Effect/Effect.hpp"
#include "XpManager.hpp"
#include "DamageType.hpp"
#include "Spell/Spell.hpp"
#include "Item.hpp"
#include "Equipment.hpp"

#include "render/ParticleManager.hpp"
#include "render/coords.hpp"

#include "util/geometry.hpp"
#include "util/random.hpp"
#include "util/reduce.hpp"
#include "util/EnumMap.hpp"

#include <SFML/Graphics/Texture.hpp>

namespace core {
	/// Base for all Actors with hp and position in world
	class Actor : public std::enable_shared_from_this<Actor> {
	public:
		/// Immutable params of the Actor
		struct Stats {
			std::string id;

			double maxHp;
			double regen;

			double maxMana = 0;
			double manaRegen = 0;

			double damage;
			double accuracy;

			double evasion;

			util::EnumMap<DamageType, double> defences;

			double turnDelay;

			double xp;

			const sf::Texture* texture = nullptr;

			bool hasRangedAttack;
			const sf::Texture* projectileTexture = nullptr;
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
		Actor(std::shared_ptr<World> world, std::shared_ptr<XpManager> xpManager,
			  std::shared_ptr<render::ParticleManager> particles,
			  util::RandomEngine* randomEngine);

		const Stats& stats() const {
			return stats_;
		}

		void stats(const Stats& newStats) {
			stats_ = newStats;
		}

		Controller& controller() noexcept {
			return *controller_;
		}

		const Controller& controller() const noexcept {
			return *controller_;
		}

		void controller(std::unique_ptr<Controller> newController) {
			controller_ = std::move(newController);
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

		void nextTurn(double newNextTurn) noexcept {
			nextTurn_ = newNextTurn;
		}

		[[nodiscard]] bool isAlive() const {
			return hp() > 0;
		}

		void beDamaged(double damage, DamageType type);

		void beBanished() {
			hp_ = 0.0;
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

		/// Sets Actor HP
		void hp(double newHp) noexcept {
			hp_ = newHp;
		}

		/// Gets Actor HP without hpMul
		[[nodiscard]] double hpUnscaled() const noexcept {
			return hp_ / hpMul;
		}

		/// Sets Actor HP without hpMul
		void hpUnscaled(double newHp) noexcept {
			hp_ = newHp * hpMul;
		}

		/// Gets max possible HP
		[[nodiscard]] double maxHp() const noexcept {
			return stats().maxHp * hpMul;
		}

		/// Gets Actor mana
		[[nodiscard]] double mana() const noexcept {
			return mana_;
		}

		/// Sets Actor mana
		void mana(double newMana) noexcept {
			mana_ = newMana;
		}

		/// Gets Actor mana without manaMul
		[[nodiscard]] double manaUnscaled() const noexcept {
			return mana_ / manaMul;
		}

		/// Sets Actor mana without manaMul
		void manaUnscaled(double newMana) noexcept {
			mana_ = newMana * manaMul;
		}

		/// Gets max possible mana
		[[nodiscard]] double maxMana() const noexcept {
			return stats().maxMana * manaMul;
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

		/// Adds mana to Actor's mana respecting maxMana
		void restoreMana(double dmana) noexcept {
			mana_ += dmana;
			mana_ = std::min(mana_, maxMana());
		}

		[[nodiscard]] const sf::Texture* texture() const noexcept {
			return stats().texture;
		}

		void endTurn(std::shared_ptr<Spell> castedSpell = nullptr) noexcept;

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

		void cancelEffects() {
			std::erase_if(effects_, [&](const auto& effect) {
				return effect->isCancelable();
			});
		}

		void addSpell(std::shared_ptr<Spell> spell) {
			spell->owner(weak_from_this());
			spells_.push_back(std::move(spell));
		}

		const std::vector<std::shared_ptr<Spell>>& spells() const noexcept {
			return spells_;
		}

		void addItem(std::unique_ptr<Item> item) {
			item->owner(weak_from_this());
			items_.push_back(std::move(item));
		}

		const std::vector<std::unique_ptr<Item>>& items() const noexcept {
			return items_;
		}

		double xpMul() const;

		void updateHp();

		bool hasRangedAttack() {
			return stats().hasRangedAttack;
		}

		void attack(Actor& other);

		void rangedAttack(Actor& other) {
			attack(other);

			auto pos1 = render::toScreen(util::geometry_cast<float>(util::getXY(position())) + sf::Vector2f{0.5f, 0.5f});
			auto pos2 = render::toScreen(util::geometry_cast<float>(util::getXY(other.position())) + sf::Vector2f{0.5f, 0.5f});
			particles->add(pos1, pos2, position().z,
				           stats().projectileFlightTime, stats().projectileTexture);
		}

		double hitChance(double accuracy) {
			return accuracy / (accuracy + evasion());
		}

		void heal(double healed) {
			hp_ += healed;
			hp_ = std::min(hp(), maxHp());
		}

		std::shared_ptr<Spell> castedSpell() const {
			return castedSpell_;
		}

		void castedSpell(std::shared_ptr<Spell> spell) {
			castedSpell_ = spell;
		}

		bool equip(Equipment& piece, EquipmentSlot slot) {
			auto& slots = equipment_[static_cast<int>(slot)];
			if (slots.empty()) {
				return false;
			}

			auto pieceIter = std::ranges::find_if(items_, [&](const auto& ptr) {
				return ptr.get() == &piece;
			});
			if (pieceIter == items_.end()) {
				return false;
			}

			std::unique_ptr<Equipment> piecePtr{static_cast<Equipment*>(pieceIter->release())};
			items_.erase(pieceIter);

			if (auto iter = std::ranges::find_if(slots, [](const auto& ptr) {
				return ptr == nullptr;
			}); iter != slots.end()) {
				*iter = std::move(piecePtr);
			} else {
				std::ranges::rotate(slots, slots.end() - 1);
				addItem(std::exchange(slots.front(), std::move(piecePtr)));
			}

			updateHp();

			return true;
		}

		void unequip(EquipmentSlot slot, int index) {
			if (auto& item = equipment_[static_cast<int>(slot)][index]) {
				addItem(static_cast<std::unique_ptr<Item>>(std::exchange(item, nullptr)));
				updateHp();
			}
		}

		std::span<const std::vector<std::unique_ptr<Equipment>>, util::nEnumerators<EquipmentSlot>> equipment() const noexcept {
			return equipment_;
		}

		void setEquipment(std::unique_ptr<Equipment> piece, EquipmentSlot slotType, int slotIndex) {
			equipment_[static_cast<int>(slotType)][slotIndex] = std::move(piece);
			updateHp();
		}

		template <typename T, typename Op, typename Proj = std::identity>
		auto reduceStatBoosters(T init, Op op = {}, Proj proj = {}) const {
			T res = std::move(init);
			for (const auto& effect : effects()) {
				res = std::invoke(op, std::move(res), std::invoke(proj, *effect));
			}
			for (const auto& piece : equipment_ | std::views::join) {
				if (piece) {
					res = std::invoke(op, std::move(res), std::invoke(proj, *piece));
				}
			}
			return res;
		}
	private:
		Stats stats_;
		std::unique_ptr<Controller> controller_;
		std::vector<std::unique_ptr<Effect>> effects_;
		std::vector<std::shared_ptr<Spell>> spells_;
		std::vector<std::unique_ptr<Item>> items_;
		std::shared_ptr<Spell> castedSpell_;
		std::array<std::vector<std::unique_ptr<Equipment>>, util::nEnumerators<EquipmentSlot>> equipment_;

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

	BOOST_DESCRIBE_STRUCT(Actor::Stats, (), (
		id, maxHp, regen, maxMana, manaRegen, 
		damage, accuracy, evasion, defences, turnDelay, xp, texture, hasRangedAttack, projectileTexture, projectileFlightTime
	))
}

#endif