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

#include <JutchsON.hpp>

#include <SFML/System/Vector3.hpp>

#include <boost/describe.hpp>

#include <vector>
#include <span>
#include <memory>

namespace render {
	class PlayerMap {
	public:
		PlayerMap(std::shared_ptr<core::World> world, std::shared_ptr<AssetManager> assets, 
				  std::shared_ptr<util::Raycaster> raycaster);

		enum class TileState {
			UNSEEN,
			MEMORIZED,
			VISIBLE,
		};

		[[nodiscard]] TileState tileState(core::Position<int> position) const noexcept {
			return tileStates[static_cast<sf::Vector3i>(position)];
		}

		/// Saves seen Actor state to draw it.
		struct SeenActor {
			core::Position<int> position;

			double hp;
			double maxHp;

			double mana;
			double maxMana;

			AiState aiState;
			const sf::Texture* texture;
		};

		[[nodiscard]] std::span<const SeenActor> seenActors() const noexcept {
			return seenActors_;
		}

		/// Saves seen Item state to draw it.
		struct SeenItem {
			core::Position<int> position;
			const sf::Texture* texture;
		};

		[[nodiscard]] std::span<const SeenItem> seenItems() const noexcept {
			return seenItems_;
		}

		[[nodiscard]] bool canSee(core::Position<int> position) const noexcept;

		void onGenerate();

		void update() {
			updateTiles();
			updateActors();
			updateItems();
		}

		void updateTiles();

		void discoverLevelTiles(int z);
		void discoverLevelActors(int z);
		void discoverLevelItems(int z);

		void addSound(core::Sound sound) {
			recentSounds_.push_back(sound);
		}

		void clearSounds() {
			recentSounds_.clear();
		}
		 
		std::span<const core::Sound> recentSounds() const noexcept {                                                                                                                                                                                                       
			return recentSounds_;
		}

		void parseTileStates(std::string_view data);
		[[nodiscard]] std::string stringifyTileStates() const;

		void parseSeenActor(JutchsON::StringView s) {
			seenActors_.push_back(*JutchsON::parse<PlayerMap::SeenActor>(s, Env{assets}));
		}

		[[nodiscard]] std::string stringifySeenActor(SeenActor actor) const {
			return JutchsON::write(actor, Env{assets});
		}

		void parseSeenItem(JutchsON::StringView s) {
			seenItems_.push_back(*JutchsON::parse<PlayerMap::SeenItem>(s, Env{assets}));
		}

		[[nodiscard]] std::string stringifySeenItem(SeenItem item) const {
			return JutchsON::write(item, Env{assets});
		}
	private:
		util::Array3D<TileState> tileStates;
		std::vector<SeenActor> seenActors_;
		std::vector<SeenItem> seenItems_;

		std::vector<core::Sound> recentSounds_;

		std::shared_ptr<core::World> world;
		std::shared_ptr<AssetManager> assets;
		std::shared_ptr<util::Raycaster> raycaster;

		void updateActors();
		void updateItems();

		struct Env {
			std::shared_ptr<AssetManager> assets;
		};
	};

	BOOST_DESCRIBE_STRUCT(PlayerMap::SeenActor, (), (position, hp, maxHp, mana, maxMana, aiState, texture))
	BOOST_DESCRIBE_STRUCT(PlayerMap::SeenItem, (), (position, texture))
}

#endif