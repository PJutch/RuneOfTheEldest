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

#ifndef TELEPORT_SPELL_HPP_
#define TELEPORT_SPELL_HPP_

#include "Spell.hpp"

#include "core/fwd.hpp"
#include "core/Position.hpp"
#include "core/World.hpp"
#include "core/Actor.hpp"

#include "render/ParticleManager.hpp"

namespace sf {
	class Texture;
}

#include <boost/describe.hpp>

#include <string>
#include <memory>

namespace core {
	/// Spell that Actor can cast
	class TeleportSpell : public Spell {
	public:
		struct Stats {
			double mana;
		};

		TeleportSpell(Stats stats_, const sf::Texture& icon, std::string_view name, std::shared_ptr<World> world_) :
			Spell{icon, name}, stats{stats_}, world{std::move(world_)} {}

		bool cast(std::shared_ptr<Actor> self, core::Position<int>) final {
			if (!self->useMana(stats.mana))
				return false;

			self->position(world->randomPositionAt(self->position().z, [](const core::World& world, sf::Vector3i pos) {
				return world.isFree(pos);
			}));

			return true;
		}

		[[nodiscard]] std::shared_ptr<Spell> clone() const final {
			return std::make_shared<TeleportSpell>(*this);
		}
	private:
		Stats stats;

		std::shared_ptr<World> world;
		std::shared_ptr<render::ParticleManager> particles;
	};

	BOOST_DESCRIBE_STRUCT(TeleportSpell::Stats, (), (mana))
}

#endif