/* This file is part of the Rune of the Eldest.
The Rune of the Eldest - Roguelike about the mage seeking for ancient knowledges
Copyright(C) 2023  PJutch

The Rune of the Eldest is free software : you can redistribute it and /or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

The Rune of the Eldest is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with the Rune of the Eldest.
If not, see < https://www.gnu.org/licenses/>. */

#include "ParticleManager.hpp"

#include "core/Position.hpp"

#include "util/geometry.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

#include <vector>
#include <cmath>

namespace render {
	void ParticleManager::update(sf::Time elapsedTime) {
		for (Particle& particle : particles)
			particle.livedTime += elapsedTime;

		for (auto& particle : customParticles)
			particle->update(elapsedTime);

		std::erase_if(particles, [](const Particle& p) {
			return p.livedTime > p.maxLifetime;
		});

		std::erase_if(customParticles, [](const auto& p) {
			return p->shouldBeDeleted();
		});
	}

	void ParticleManager::draw(sf::RenderTarget& target, core::Position<float> cameraPos) const {
		for (const Particle& particle : particles) {
			auto pos = util::lerp(particle.firstPos, particle.lastPos, particle.livedTime / particle.maxLifetime);
			drawParticle(target, cameraPos, {pos, particle.z}, particle.rotation, particle.texture);
		}

		for (auto& particle : customParticles)
			particle->draw(target, cameraPos);
	}

	void ParticleManager::drawParticle(sf::RenderTarget& target,
			core::Position<float> cameraPos, core::Position<float> pos,
			float rotation, const sf::Texture* texture) const {
		if (pos.z != cameraPos.z)
			return;

		sf::Sprite sprite;

		sprite.setTexture(*texture);
		sprite.setPosition(pos.xy());
		sprite.setOrigin(util::geometry_cast<float>(texture->getSize()) / 2.f);
		sprite.setRotation(rotation);

		target.draw(sprite);
	}
}
