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

#ifndef BRANCHING_RAY_SPELL_HPP_
#define BRANCHING_RAY_SPELL_HPP_

#include "Spell.hpp"

#include "ActorImpact.hpp"

#include "core/fwd.hpp"
#include "core/DamageType.hpp"
#include "core/Position.hpp"
#include "core/World.hpp"
#include "core/Actor.hpp"

#include "render/ParticleManager.hpp"
#include "render/coords.hpp"

#include "util/raycast.hpp"
#include "util/random.hpp"

namespace sf {
	class Texture;
}

#include <boost/describe.hpp>

#include <string>
#include <memory>

namespace core {
	namespace Spells {
		class BranchingRay : public Spell {
		public:
			struct Data {
				const sf::Texture* icon = nullptr;
				std::string name;

				ActorImpact impact;

				double mana;

				double chainChance;

				sf::Time visibleTime;
				const sf::Texture* rayTexture = nullptr;
			};

			BranchingRay(Data data_, const auto& env) :
				Spell{*data_.icon, env.id, data_.name}, data{data_}, world{env.world},
				particles{env.particles}, raycaster{env.raycaster}, randomEngine{env.randomEngine} {}

			UsageResult cast(core::Position<int> target, bool useMana = true) final {
				auto owner_ = owner();

				auto other = world->actorAt(target);
				if (!other)
					return UsageResult::FAILURE;

				if (useMana && !owner_->useMana(data.mana))
					return UsageResult::FAILURE;

				return attack(*owner_, *other) ? UsageResult::SUCCESS : UsageResult::FAILURE;
			}

			[[nodiscard]] std::shared_ptr<Spell> clone() const final {
				return std::make_shared<BranchingRay>(*this);
			}
		private:
			Data data;

			std::shared_ptr<World> world;
			std::shared_ptr<render::ParticleManager> particles;
			std::shared_ptr<util::Raycaster> raycaster;
			util::RandomEngine* randomEngine;

			void spawnRay(core::Position<int> prev, core::Position<int> next) {
				auto pos1 = render::toScreen(util::geometry_cast<float>(prev.xy()) + sf::Vector2f{0.5f, 0.5f});
				auto pos2 = render::toScreen(util::geometry_cast<float>(next.xy()) + sf::Vector2f{0.5f, 0.5f});

				float polarAnlge = util::polarAngle(pos2 - pos1);
				float rotation = util::toDegrees(polarAnlge) + 90.f;

				float distance = util::distance(pos1, pos2);

				auto segmentLen = static_cast<float>(data.rayTexture->getSize().y);
				for (float d = 0; d < distance; d += segmentLen) {
					sf::Vector2f pos = util::lerp(pos1, pos2, d / distance);
					particles->add(pos, prev.z, rotation, data.visibleTime, data.rayTexture);
				}
			}

			bool attack(Actor& prev, Actor& next) {
				if (!raycaster->canSee(prev.position(), next.position())) {
					return false;
				}

				data.impact.apply(next);
				world->makeSound({Sound::Type::ATTACK, true, prev.position()});
				spawnRay(core::Position<int>{prev.position()}, core::Position<int>{next.position()});

				for (const auto& actor : world->actors())
					if (actor.get() != &next && actor.get() != &prev && actor->isAlive() && actor->position().z == next.position().z
						&& std::bernoulli_distribution{data.chainChance}(*randomEngine))
						attack(next, *actor);
				return true;
			}
		};

		BOOST_DESCRIBE_STRUCT(BranchingRay::Data, (), (icon, name, impact, mana, chainChance, visibleTime, rayTexture))
	}
}

#endif