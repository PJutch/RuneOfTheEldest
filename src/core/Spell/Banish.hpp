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

#ifndef BANISH_HPP_
#define BANISH_HPP_

#include "Spell.hpp"

#include "core/fwd.hpp"
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
	class BanishSpell : public Spell {
	public:
		struct Stats {
			double manaPerHp;

			sf::Time visibleTime;
			const sf::Texture* particleTexture = nullptr;
		};

		BanishSpell(Stats stats_, const sf::Texture& icon, std::string_view id, std::string_view name,
				std::shared_ptr<World> world_, std::shared_ptr<render::ParticleManager> particles_) :
			Spell{icon, id, name}, stats{stats_}, world{std::move(world_)}, particles{std::move(particles_)} {}

		UsageResult cast(core::Position<int> target, bool useMana = true) final {
			auto other = world->actorAt(target);
			if (!other)
				return UsageResult::FAILURE;

			if (useMana && !owner()->useMana(stats.manaPerHp * other->hp()))
				return UsageResult::FAILURE;

			other->beBanished();

			world->makeSound({Sound::Type::ATTACK, true, owner()->position()});
			spawnParticle(target);

			return UsageResult::SUCCESS;
		}

		[[nodiscard]] std::shared_ptr<Spell> clone() const final {
			return std::make_shared<BanishSpell>(*this);
		}
	private:
		Stats stats;

		std::shared_ptr<World> world;
		std::shared_ptr<render::ParticleManager> particles;

		void spawnParticle(core::Position<int> target) {
			auto pos = render::toScreen(util::geometry_cast<float>(target.xy()) + sf::Vector2f{0.5f, 0.5f});

			particles->add(pos, target.z, stats.visibleTime, stats.particleTexture);
		}
	};

	BOOST_DESCRIBE_STRUCT(BanishSpell::Stats, (), (manaPerHp, visibleTime, particleTexture))
}

#endif