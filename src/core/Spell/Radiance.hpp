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

#ifndef RADIANCE_SPELL_HPP_
#define RADIANCE_SPELL_HPP_

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

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

#include <boost/describe.hpp>

#include <string>
#include <memory>

namespace core {
	class RadianceSpell : public Spell {
	public:
		struct Stats {
			ActorImpact impact;

			double mana;

			sf::Time visibleTime;
			const sf::Texture* tileTexture = nullptr;
		};

		RadianceSpell(Stats stats_, const sf::Texture& icon, std::string_view name,
			          std::shared_ptr<World> world_, std::shared_ptr<render::ParticleManager> particles_,
					  std::shared_ptr<util::Raycaster> raycaster_) :
			Spell{icon, name}, stats{stats_}, world{std::move(world_)}, 
			particles{std::move(particles_)}, raycaster{std::move(raycaster_)} {}

		CastResult cast(std::shared_ptr<Actor> self) final {
			if (!self->useMana(stats.mana))
				return CastResult::FAILURE;

			world->makeSound({Sound::Type::ATTACK, true, self->position()});

			for (const auto& actor : world->actors())
				if (actor.get() != self.get() && raycaster->canSee(self->position(), actor->position())) {
					stats.impact.apply(*actor);
				}

			spawnAura(core::Position<int>{self->position()});

			return CastResult::SUCCESS;
		}

		[[nodiscard]] std::shared_ptr<Spell> clone() const final {
			return std::make_shared<RadianceSpell>(*this);
		}
	private:
		Stats stats;

		std::shared_ptr<World> world;
		std::shared_ptr<render::ParticleManager> particles;
		std::shared_ptr<util::Raycaster> raycaster;

		void spawnAura(core::Position<int> self) {
			for (int x = 0; x < world->tiles().shape().x; ++x)
				for (int y = 0; y < world->tiles().shape().y; ++y) {
					if (raycaster->canSee(static_cast<sf::Vector3i>(self), {x, y, self.z})) {
						sf::Vector2f pos = render::toScreen(sf::Vector2f{x + 0.5f, y + 0.5f});
						particles->add(pos, self.z, stats.visibleTime, stats.tileTexture);
					}
				}
		}
	};

	BOOST_DESCRIBE_STRUCT(RadianceSpell::Stats, (), (impact, mana, visibleTime, tileTexture))
}

#endif