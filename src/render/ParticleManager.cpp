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

		std::erase_if(particles, [](const Particle& p) {
			return p.livedTime > p.maxLifetime;
		});
	}

	void ParticleManager::draw(sf::RenderTarget& target, core::Position<float> cameraPos) const {
		for (const Particle& particle : particles) {
			if (particle.z != cameraPos.z)
				return;

			sf::Sprite sprite;
			sprite.setTexture(*particle.texture);

			sprite.setPosition(util::lerp(particle.firstPos, particle.lastPos,
										  particle.livedTime / particle.maxLifetime));

			sprite.setOrigin(util::geometry_cast<float>(particle.texture->getSize()) / 2.f);

			sprite.setRotation(particle.rotation);

			target.draw(sprite);
		}
	}
}
