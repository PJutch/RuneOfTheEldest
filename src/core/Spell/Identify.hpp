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
	namespace Spells {
		class Identify : public Spell {
		public:
			struct Data {
				const sf::Texture* icon;
				std::string name;

				double mana;
			};

			Identify(Data data_, const auto& env) :
				Spell{*data_.icon, env.id, data_.name}, data{data_} {}

			UsageResult cast(Item& target, bool useMana = true) final {
				if (useMana && !owner()->useMana(data.mana))
					return UsageResult::FAILURE;

				target.identify();
				return UsageResult::SUCCESS;
			}

			[[nodiscard]] std::shared_ptr<Spell> clone() const final {
				return std::make_shared<Identify>(*this);
			}
		private:
			Data data;

			std::shared_ptr<render::ParticleManager> particles;
		};

		BOOST_DESCRIBE_STRUCT(Identify::Data, (), (icon, name, mana))
	}
}

#endif