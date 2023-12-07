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

#ifndef EFFECT_RING_HPP_
#define EFFECT_RING_HPP_

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

#include "SFML/Graphics/Sprite.hpp"

#include <string>
#include <memory>

namespace core {
	class EffectRingSpell : public Spell {
	public:
		struct Stats {
			const Effect* effect;

			double accuracy;

			double radius;

			double mana;

			sf::Time visibleTime;
			const sf::Texture* texture = nullptr;
		};

		EffectRingSpell(Stats stats_, const sf::Texture& icon, std::string_view name,
			std::shared_ptr<World> world_, std::shared_ptr<render::ParticleManager> particles_,
			std::shared_ptr<util::Raycaster> raycaster_, util::RandomEngine& randomEngine_) :
			Spell{icon, name}, stats{stats_}, world{std::move(world_)},
			particles{std::move(particles_)}, raycaster{std::move(raycaster_)}, randomEngine{&randomEngine_} {}

		bool cast(std::shared_ptr<Actor> self, core::Position<int>) final {
			if (!self->useMana(stats.mana)) {
				return false;
			}

			for (const auto& actor : world->actors()) {
				if (actor.get() != self.get()
				 && util::distance(util::getXY(actor->position()), util::getXY(self->position())) <= stats.radius
				 && raycaster->canSee(self->position(), actor->position())
				 && std::bernoulli_distribution{actor->hitChance(stats.accuracy)}(*randomEngine)) {
					actor->addEffect(stats.effect->clone());
					world->makeSound({Sound::Type::ATTACK, true, actor->position()});
				}
			}

			spawnRing(static_cast<core::Position<int>>(self->position()));

			return true;
		}

		[[nodiscard]] std::shared_ptr<Spell> clone() const final {
			return std::make_shared<EffectRingSpell>(*this);
		}
	private:
		class Ring : public render::ParticleManager::CustomParticle {
		public:
			Ring(core::Position<float> position_, float maxTileRadius_,
				 sf::Time maxLifetime_, const sf::Texture* texture_) :
				position{position_}, maxLifetime{maxLifetime_}, maxTileRadius{maxTileRadius_},
				texture{texture_} {}

			void update(sf::Time elapsedTime) final {
				lifetime += elapsedTime;
			}

			void draw(sf::RenderTarget& target, core::Position<float> cameraPos) const {
				if (position.z != cameraPos.z)
					return;

				sf::Sprite sprite(*texture);

				sprite.setPosition(position.xy());
				sprite.setOrigin(util::geometry_cast<float>(texture->getSize()) / 2.f);
				sprite.setScale(scale());

				target.draw(sprite);
			}

			bool shouldBeDeleted() const {
				return lifetime >= maxLifetime;
			}
		private:
			core::Position<float> position;
			sf::Time maxLifetime;
			const sf::Texture* texture;
			float maxTileRadius;

			sf::Time lifetime;

			sf::Vector2f scale() const {
				float tileRadius = lifetime / maxLifetime * maxTileRadius;
				sf::Vector2f pixelSize = 2 * tileRadius * util::geometry_cast<float>(render::tileSize);
				auto textureSize = util::geometry_cast<float>(texture->getSize());
				return {pixelSize.x / textureSize.x, pixelSize.y / textureSize.y};
			}
		};

		Stats stats;

		std::shared_ptr<World> world;
		std::shared_ptr<render::ParticleManager> particles;
		std::shared_ptr<util::Raycaster> raycaster;
		util::RandomEngine* randomEngine;

		void spawnRing(core::Position<int> self) {
			auto pos = render::toScreen(util::geometry_cast<float>(self.xy()) + sf::Vector2f{0.5f, 0.5f});
			particles->add(std::make_unique<Ring>(core::Position<float>{pos, self.z}, stats.radius,
						   stats.visibleTime, stats.texture));
		}
	};

	BOOST_DESCRIBE_STRUCT(EffectRingSpell::Stats, (), (
		effect, accuracy, radius, mana, visibleTime, texture
	))
}

#endif