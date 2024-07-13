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

#ifndef CHARGING_RAY_SPELL_HPP_
#define CHARGING_RAY_SPELL_HPP_

#include "Spell.hpp"

#include "core/fwd.hpp"
#include "core/DamageType.hpp"
#include "core/Position.hpp"
#include "core/World.hpp"
#include "core/Actor.hpp"

#include "render/ParticleManager.hpp"
#include "render/coords.hpp"

#include "util/raycast.hpp"
#include "util/random.hpp"
#include "util/parse.hpp"
#include "util/parseKeyValue.hpp"
#include "util/stringify.hpp"

namespace sf {
	class Texture;
}

#include <boost/describe.hpp>

#include <string>
#include <memory>

namespace core {
	class ChargingRaySpell : public Spell, public std::enable_shared_from_this<ChargingRaySpell> {
	public:
		struct Stats {
			const sf::Texture* icon = nullptr;
			std::string name;

			double damage;
			DamageType damageType;
			double damageGrowthMul;

			double accuracy;

			double mana;

			sf::Time minVisibleTime;
			const sf::Texture* rayTexture = nullptr;
		};

		ChargingRaySpell(Stats stats_, const auto& env) :
			Spell{*stats_.icon, env.id, stats_.name}, stats{stats_}, world{env.world},
			particles{env.particles}, raycaster{env.raycaster} {}

		UsageResult cast(core::Position<int> targetPos, bool useMana_ = false) final {
			auto target_ = world->actorAt(targetPos);
			if (!target_)
				return UsageResult::FAILURE;

			if (owner()->castedSpell().get() != this || useMana != useMana_ || target_ != target.lock()) {
				target = std::move(target_);
				damageMul = 1;
				particles->add(std::make_unique<Ray>(shared_from_this(), particles));
			}
			return attack() ? UsageResult::SUCCESS : UsageResult::FAILURE;
		}

		bool continueCast() final {
			return isCasted() && attack();
		}

		void restartCast() final {
			particles->add(std::make_unique<Ray>(shared_from_this(), particles));
		}

		[[nodiscard]] std::shared_ptr<Spell> clone() const final {
			return std::make_shared<ChargingRaySpell>(*this);
		}

		void parseData(std::string_view data) final {
			util::KeyValueVisitor visitor;

			visitor.key("damageMul").unique().required().callback([&](std::string_view data) {
				damageMul = util::parseReal(data);
			});

			visitor.key("targetPosition").unique().callback([&](std::string_view data) {
				target = world->actorAt(util::parseVector3i(data));
			});

			util::forEackInlineKeyValuePair(data, visitor);
			visitor.validate();
		}

		[[nodiscard]] std::string stringify() const final {
			std::string result = std::format("{} damageMul {}", id(), damageMul);
			if (!target.expired()) {
				result += std::format(", targetPosition {}", util::stringifyVector3(target.lock()->position()));
			}
			return result;
		}

		sf::Color frameColor() const final {
			return isCasted() && canAttack() ? sf::Color::Green : sf::Color{128, 128, 128};
		}
	private:
		Stats stats;

		std::weak_ptr<Actor> target;
		double damageMul = 1;

		bool useMana = true;

		std::shared_ptr<World> world;
		std::shared_ptr<render::ParticleManager> particles;
		std::shared_ptr<util::Raycaster> raycaster;
		util::RandomEngine* randomEngine = nullptr;

		class Ray : public render::ParticleManager::CustomParticle {
		public:
			Ray(std::shared_ptr<ChargingRaySpell> spell_, std::weak_ptr<render::ParticleManager> particleManager_) : 
					spell{std::move(spell_)}, particleManager {std::move(particleManager_)} {
				if (!spell->target.expired())
					targetPos = spell->target.lock()->position();
			}

			void update(sf::Time elapsedTime) final {
				lifetime += elapsedTime;
				if (!spell->target.expired())
					targetPos = spell->target.lock()->position();
			}

			void draw(sf::RenderTarget& target, core::Position<float> cameraPos) const {
				auto selfPos = spell->owner()->position();

				auto pos1 = render::toScreen(util::geometry_cast<float>(util::getXY(selfPos)) + sf::Vector2f{0.5f, 0.5f});
				auto pos2 = render::toScreen(util::geometry_cast<float>(util::getXY(targetPos)) + sf::Vector2f{0.5f, 0.5f});

				float polarAnlge = util::polarAngle(pos2 - pos1);
				float rotation = util::toDegrees(polarAnlge) + 90.f;

				float distance = util::distance(pos1, pos2);

				float segmentLen = spell->stats.rayTexture->getSize().y;
				for (float d = 0; d < distance; d += segmentLen) {
					sf::Vector2f pos = util::lerp(pos1, pos2, d / distance);
					particleManager.lock()->drawParticle(target, cameraPos, {pos, selfPos.z}, rotation, spell->stats.rayTexture);
				}
			}

			bool shouldBeDeleted() const {
				return (!spell->isCasted() || !spell->canAttack())
					&& lifetime >= spell->stats.minVisibleTime;
			}
		private:
			std::shared_ptr<ChargingRaySpell> spell;
			std::weak_ptr<render::ParticleManager> particleManager;
			
			sf::Time lifetime;

			sf::Vector3i targetPos;
		};

		bool canAttack() const {
			if (target.expired()) {
				return false;
			}

			auto target_ = target.lock();

			return target_->isAlive()
				&& raycaster->canSee(owner()->position(), target_->position())
				&& owner()->mana() >= stats.mana;
		}

		bool isCasted() const {
			return owner()->castedSpell().get() == this;
		}

		bool attack() {
			if (!canAttack()) {
				return false;
			}

			auto target_ = target.lock();

			if (useMana && !owner()->useMana(stats.mana)) {
				return false;
			}

			target_->beAttacked(damageMul * stats.damage, stats.accuracy, stats.damageType);
			world->makeSound({Sound::Type::ATTACK, true, target_->position()});

			damageMul *= stats.damageGrowthMul;

			return true;
		}
	};

	BOOST_DESCRIBE_STRUCT(ChargingRaySpell::Stats, (), (
		icon, name, damage, damageType, damageGrowthMul, accuracy, mana, minVisibleTime, rayTexture
	))
}

#endif