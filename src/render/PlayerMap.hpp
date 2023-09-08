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

#ifndef SEEN_TILES_HPP_
#define SEEN_TILES_HPP_

#include "core/fwd.hpp"
#include "core/Actor.hpp"

#include "util/Array3D.hpp"
#include "util/raycast.hpp"

#include <SFML/System/Vector3.hpp>

#include <vector>
#include <span>
#include <memory>

namespace render {
	class PlayerMap {
	public:
		PlayerMap(std::shared_ptr<core::World> world, std::shared_ptr<util::Raycaster> raycaster);

		enum class TileState {
			UNSEEN,
			MEMORIZED,
			VISIBLE,
		};

		/// Saves seen Actor state to draw it.
		struct SeenActor {
			sf::Vector3i position;
			double hp;
			double maxHp;
			AiState aiState;
			const sf::Texture* texture;
		};

		[[nodiscard]] TileState tileState(sf::Vector3i position) const noexcept {
			return tileStates[position];
		}

		[[nodiscard]] std::span<const SeenActor> seenActors() const noexcept {
			return seenActors_;
		}

		[[nodiscard]] bool canSee(sf::Vector3i position) const noexcept {
			return seeEverything()
				|| raycaster->canSee(world->player().position(), position);
		}

		void onGenerate();

		void update() {
			updateTiles();
			updateActors();
		}

		void handleSound(core::Sound sound) {
			recentSounds_.push_back(sound);
		}

		void clearSounds() {
			recentSounds_.clear();
		}

		void seeEverything(bool newSeeEverything) noexcept {
			seeEverything_ = newSeeEverything;
		}

		bool seeEverything() const noexcept {
			return seeEverything_;
		}

		std::span<const core::Sound> recentSounds() const noexcept {
			return recentSounds_;
		}
	private:
		bool seeEverything_ = false;

		util::Array3D<TileState> tileStates;
		std::vector<SeenActor> seenActors_;

		std::vector<core::Sound> recentSounds_;

		std::shared_ptr<core::World> world;
		std::shared_ptr<util::Raycaster> raycaster;

		void updateTiles();
		void updateActors();
	};
}

#endif