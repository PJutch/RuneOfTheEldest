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

#ifndef PROJECTILE_SPELL_HPP_
#define PROJECTILE_SPELL_HPP_

#include "Spell.hpp"

#include "ActorImpact.hpp"

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

#include <boost/describe.hpp>

#include <string>
#include <memory>

namespace core {
	/// Spell that Actor can cast
	class ProjectileSpell : public Spell {
	public:
		struct Stats {
			ActorImpact impact;

			double mana;

			sf::Time flightTime;
			const sf::Texture* projectileTexture = nullptr;
		};

		ProjectileSpell(Stats stats_, const sf::Texture& icon, std::string_view id, std::string_view name,
			            std::shared_ptr<World> world_, std::shared_ptr<render::ParticleManager> particles_) :
			Spell{icon, id, name}, stats{stats_}, world{std::move(world_)}, particles{std::move(particles_)} {}

		UsageResult cast(core::Position<int> target, bool useMana = true) final {
			auto other = world->actorAt(target);
			if (!other)
				return UsageResult::FAILURE;

			if (useMana && !owner()->useMana(stats.mana))
				return UsageResult::FAILURE;

			stats.impact.apply(*other);
			world->makeSound({Sound::Type::ATTACK, true, owner()->position()});
			spawnParticle(core::Position<int>{owner()->position()}, target);

			return UsageResult::SUCCESS;
		}

		[[nodiscard]] std::shared_ptr<Spell> clone() const final {
			return std::make_shared<ProjectileSpell>(*this);
		}
	private:
		Stats stats;

		std::shared_ptr<World> world;
		std::shared_ptr<render::ParticleManager> particles;

		void spawnParticle(core::Position<int> self, core::Position<int> target) {
			auto pos1 = render::toScreen(util::geometry_cast<float>(self.xy()) + sf::Vector2f{0.5f, 0.5f});
			auto pos2 = render::toScreen(util::geometry_cast<float>(target.xy()) + sf::Vector2f{0.5f, 0.5f});
			particles->add(pos1, pos2, self.z, stats.flightTime, stats.projectileTexture);
		}
	};

	BOOST_DESCRIBE_STRUCT(ProjectileSpell::Stats, (), (impact, mana, flightTime, projectileTexture))
}

#endif