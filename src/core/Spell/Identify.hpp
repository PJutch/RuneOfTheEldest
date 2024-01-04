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

#ifndef IDENTIFY_SPELL_HPP_
#define IDENTIFY_SPELL_HPP_

#include "Spell.hpp"

#include "core/fwd.hpp"
#include "core/Position.hpp"
#include "core/World.hpp"
#include "core/Actor.hpp"
#include "core/Item.hpp"

#include "render/ParticleManager.hpp"

namespace sf {
	class Texture;
}

#include <boost/describe.hpp>

#include <string>
#include <memory>

namespace core {
	/// Spell that Actor can cast
	class IdentifySpell : public Spell {
	public:
		struct Stats {
			double mana;
		};

		IdentifySpell(Stats stats_, const sf::Texture& icon, std::string_view id, std::string_view name,
					  std::shared_ptr<World> world_) :
			Spell{icon, id, name}, stats{stats_} {}

		UsageResult cast(Item& target, bool useMana = true) final {
			if (useMana && !owner()->useMana(stats.mana))
				return UsageResult::FAILURE;

			target.identify();
			return UsageResult::SUCCESS;
		}

		[[nodiscard]] std::shared_ptr<Spell> clone() const final {
			return std::make_shared<IdentifySpell>(*this);
		}
	private:
		Stats stats;

		std::shared_ptr<render::ParticleManager> particles;
	};

	BOOST_DESCRIBE_STRUCT(IdentifySpell::Stats, (), (mana))
}

#endif