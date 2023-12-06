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
	class BranchingRaySpell : public Spell {
	public:
		struct Stats {
			double damage;
			DamageType damageType;

			double accuracy;

			double mana;

			double chainChance;

			sf::Time visibleTime;
			const sf::Texture* rayTexture;
		};

		BranchingRaySpell(Stats stats_, const sf::Texture& icon, std::string_view name,
			              std::shared_ptr<World> world_, std::shared_ptr<render::ParticleManager> particles_,
						  std::shared_ptr<util::Raycaster> raycaster_, util::RandomEngine& randomEngine_) :
			Spell{icon, name}, stats{stats_}, world{std::move(world_)}, 
			particles{std::move(particles_)}, raycaster{std::move(raycaster_)}, randomEngine{&randomEngine_} {}

		bool cast(std::shared_ptr<Actor> self, core::Position<int> target) final {
			auto other = world->actorAt(target);
			if (!other)
				return false;

			if (!self->useMana(stats.mana))
				return false;

			attack(*self, *other);

			return true;
		}

		[[nodiscard]] std::shared_ptr<Spell> clone() const final {
			return std::make_shared<BranchingRaySpell>(*this);
		}
	private:
		Stats stats;

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

			float segmentLen = stats.rayTexture->getSize().y;
			for (float d = 0; d < distance; d += segmentLen) {
				sf::Vector2f pos = util::lerp(pos1, pos2, d / distance);
				particles->add(pos, prev.z, rotation, stats.visibleTime, stats.rayTexture);
			}
		}

		void attack(Actor& prev, Actor& next) {
			if (!raycaster->canSee(prev.position(), next.position())) {
				return;
			}

			next.beAttacked(stats.damage, stats.accuracy, stats.damageType);
			world->makeSound({Sound::Type::ATTACK, true, prev.position()});
			spawnRay(core::Position<int>{prev.position()}, core::Position<int>{next.position()});

			for (const auto& actor : world->actors())
				if (actor.get() != &next && actor.get() != &prev && actor->isAlive() && actor->position().z == next.position().z
				 && std::bernoulli_distribution{stats.chainChance}(*randomEngine))
					attack(next, *actor);
		}
	};

	BOOST_DESCRIBE_STRUCT(BranchingRaySpell::Stats, (), (
		damage, damageType, accuracy, mana, chainChance, visibleTime, rayTexture
	))
}

#endif