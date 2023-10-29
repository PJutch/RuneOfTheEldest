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

#ifndef PARTICLE_MANAGER_HPP_
#define PARTICLE_MANAGER_HPP_

#include "core/Position.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>

#include <vector>

namespace render {
	class ParticleManager {
	public:
		void add(sf::Vector2f firstPos, sf::Vector2f lastPos, int z, sf::Time maxLifetime, const sf::Texture* texture) {
			particles.emplace_back(firstPos, lastPos, z, sf::Time::Zero, maxLifetime, texture);
		}

		void update(sf::Time elapsedTime);

		void draw(sf::RenderTarget& target, core::Position<float> cameraPos) const;

		void clear() {
			particles.clear();
		}
	private:
		struct Particle {
			sf::Vector2f firstPos;
			sf::Vector2f lastPos;
			int z;

			sf::Time livedTime;
			sf::Time maxLifetime;

			const sf::Texture* texture;
		};
		std::vector<Particle> particles;
	};
}

#endif