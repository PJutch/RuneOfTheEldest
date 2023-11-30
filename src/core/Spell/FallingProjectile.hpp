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

#ifndef FALLING_PROJECTILE_SPELL_HPP_
#define FALLING_PROJECTILE_SPELL_HPP_

#include "Spell.hpp"

#include "core/fwd.hpp"
#include "core/DamageType.hpp"
#include "core/Position.hpp"
#include "core/World.hpp"
#include "core/Actor.hpp"

#include "render/ParticleManager.hpp"
#include "render/coords.hpp"

namespace sf {
	class Texture;
}

#include <string>
#include <memory>

namespace core {
	/// Spell that Actor can cast
	class FallingProjectileSpell : public Spell {
	public:
		struct Stats {
			double damage;
			DamageType damageType;

			double accuracy;

			double manaUsage;

			float fallHeight;
			sf::Time fallTime;
			const sf::Texture* projectileTexture;
		};

		FallingProjectileSpell(Stats stats_, const sf::Texture& icon, std::string_view name,
			                   std::shared_ptr<World> world_, std::shared_ptr<render::ParticleManager> particles_) :
			Spell{icon, name}, stats{stats_}, world{std::move(world_)}, particles{std::move(particles_)} {}

		bool cast(Actor& self, core::Position<int> target) final {
			auto other = world->actorAt(target);
			if (!other)
				return false;

			if (!self.useMana(stats.manaUsage))
				return false;

			other->beAttacked(stats.damage, stats.accuracy, stats.damageType);
			world->makeSound({Sound::Type::ATTACK, true, self.position()});
			spawnParticle(target);

			return true;
		}
	private:
		Stats stats;

		std::shared_ptr<World> world;
		std::shared_ptr<render::ParticleManager> particles;

		void spawnParticle(core::Position<int> target) {
			auto pos2 = render::toScreen(util::geometry_cast<float>(target.xy()) + sf::Vector2f{0.5f, 0.5f});
			auto pos1 = pos2;
			pos1.y -= stats.fallHeight;

			particles->add(pos1, pos2, target.z, stats.fallTime, stats.projectileTexture);
		}
	};
}

#endif