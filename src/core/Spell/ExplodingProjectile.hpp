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

#ifndef EXPLODING_PROJECTILE_SPELL_HPP_
#define EXPLODING_PROJECTILE_SPELL_HPP_

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

#include "SFML/Graphics/Sprite.hpp"

#include <boost/describe.hpp>

#include <string>
#include <memory>

namespace core {
	namespace Spells {
		class ExplodingProjectile : public Spell {
		public:
			struct Data {
				const sf::Texture* icon;
				std::string name;

				ActorImpact impact;

				double explosionRadius;

				double mana;

				sf::Time flightTime;
				const sf::Texture* projectileTexture = nullptr;

				sf::Time explosionFrameLength;
				const sf::Texture* explosionAnimation = nullptr;
			};

			ExplodingProjectile(Data data_, const auto& env) :
				Spell{*data_.icon, env.id, data_.name}, data{data_}, world{env.world},
				particles{env.particles}, raycaster{env.raycaster} {}

			UsageResult cast(core::Position<int> target, bool useMana = true) final {
				if (!raycaster->canSee(owner()->position(), static_cast<sf::Vector3i>(target)) || useMana && !owner()->useMana(data.mana)) {
					return UsageResult::FAILURE;
				}

				for (const auto& actor : world->actors()) {
					if (util::distance(util::getXY(actor->position()), target.xy()) <= data.explosionRadius
						&& raycaster->canSee(static_cast<sf::Vector3i>(target), actor->position())) {
						data.impact.apply(*actor);
						world->makeSound({Sound::Type::ATTACK, true, actor->position()});
					}
				}

				spawnParticles(static_cast<core::Position<int>>(owner()->position()), target);

				return UsageResult::SUCCESS;
			}

			[[nodiscard]] std::shared_ptr<Spell> clone() const final {
				return std::make_shared<ExplodingProjectile>(*this);
			}
		private:
			class Explosion : public render::ParticleManager::CustomParticle {
			public:
				Explosion(core::Position<float> position_, float tileRadius_,
					sf::Time delay, sf::Time frameLength_, const sf::Texture* animation_) :
					position{position_},
					frameLength{frameLength_}, animation{animation_}, tileRadius{tileRadius_}, lifetime{-delay} {}

				void update(sf::Time elapsedTime) final {
					lifetime += elapsedTime;
				}

				void draw(sf::RenderTarget& target, core::Position<float> cameraPos) const {
					if (lifetime < sf::Time::Zero || position.z != cameraPos.z)
						return;

					sf::Sprite sprite(*animation, frameRect());

					sprite.setPosition(position.xy());
					sprite.setOrigin(textureRadius(), textureRadius());
					sprite.setScale(tileRadius / textureRadius() * util::geometry_cast<float>(render::tileSize));

					target.draw(sprite);
				}

				bool shouldBeDeleted() const {
					return lifetime >= duration();
				}
			private:
				core::Position<float> position;
				sf::Time frameLength;
				const sf::Texture* animation;
				float tileRadius;

				sf::Time lifetime;

				sf::IntRect frameRect() const {
					int frame = lifetime / frameLength;

					auto [animationX, animationY] = util::geometry_cast<int>(animation->getSize());
					return {frame * animationY, 0, animationY, animationY};
				}

				sf::Time duration() const {
					auto [animationX, animationY] = animation->getSize();
					sf::Int64 frameCount = animationX / animationY + 1;
					return frameLength * frameCount;
				}

				float textureRadius() const {
					return static_cast<float>(animation->getSize().y) / 2.f;
				}
			};

			Data data;

			std::shared_ptr<World> world;
			std::shared_ptr<render::ParticleManager> particles;
			std::shared_ptr<util::Raycaster> raycaster;

			void spawnParticles(core::Position<int> self, core::Position<int> target) {
				auto pos1 = render::toScreen(util::geometry_cast<float>(self.xy()) + sf::Vector2f{0.5f, 0.5f});
				auto pos2 = render::toScreen(util::geometry_cast<float>(target.xy()) + sf::Vector2f{0.5f, 0.5f});
				particles->add(pos1, pos2, self.z, data.flightTime, data.projectileTexture);

				particles->add(std::make_unique<Explosion>(core::Position<float>{pos2, target.z}, data.explosionRadius,
					data.flightTime, data.explosionFrameLength, data.explosionAnimation));
			}
		};

		BOOST_DESCRIBE_STRUCT(ExplodingProjectile::Data, (), (
			icon, name, impact, explosionRadius, mana,
			flightTime, projectileTexture, explosionFrameLength, explosionAnimation
		))
	}
}

#endif