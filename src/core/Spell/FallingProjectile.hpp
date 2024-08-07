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
	namespace Spells {
		/// Spell that Actor can cast
		class FallingProjectile : public Spell {
		public:
			struct Data {
				const sf::Texture* icon;
				std::string name;

				ActorImpact impact;

				double mana;

				float fallHeight;
				sf::Time fallTime;
				const sf::Texture* projectileTexture = nullptr;
			};

			FallingProjectile(Data data_, const auto& env) :
				Spell{*data_.icon, env.id, data_.name}, data{data_}, world{env.world},
				particles{env.particles}, raycaster{env.raycaster} {}

			UsageResult cast(core::Position<int> target, bool useMana = true) final {
				auto other = world->actorAt(target);
				if (!other)
					return UsageResult::FAILURE;

				if (!raycaster->canSee(owner()->position(), static_cast<sf::Vector3i>(target))
					|| useMana && !owner()->useMana(data.mana))
					return UsageResult::FAILURE;

				data.impact.apply(*other);
				world->makeSound({Sound::Type::ATTACK, true, owner()->position()});
				spawnParticle(target);

				return UsageResult::SUCCESS;
			}

			[[nodiscard]] std::shared_ptr<Spell> clone() const final {
				return std::make_shared<FallingProjectile>(*this);
			}
		private:
			Data data;

			std::shared_ptr<World> world;
			std::shared_ptr<render::ParticleManager> particles;
			std::shared_ptr<util::Raycaster> raycaster;

			void spawnParticle(core::Position<int> target) {
				auto pos2 = render::toScreen(util::geometry_cast<float>(target.xy()) + sf::Vector2f{0.5f, 0.5f});
				auto pos1 = pos2;
				pos1.y -= data.fallHeight;

				particles->add(pos1, pos2, target.z, data.fallTime, data.projectileTexture);
			}
		};

		BOOST_DESCRIBE_STRUCT(FallingProjectile::Data, (), (icon, name, impact, mana, fallHeight, fallTime, projectileTexture))
	}
}

#endif