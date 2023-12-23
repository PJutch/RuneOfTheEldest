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

#ifndef WORLD_HPP_
#define WORLD_HPP_

#include "Tile.hpp"
#include "Sound.hpp"
#include "fwd.hpp"
#include "Position.hpp"

#include "util/Array3D.hpp"
#include "util/Map.hpp"
#include "util/random.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector3.hpp>

#include <queue>
#include <span>

namespace core {
	/// Dungeons and all objects in it
	class World {
	public:
		World() = default;
		World(util::RandomEngine& randomEngine_) : randomEngine{ &randomEngine_ } {}

		[[nodiscard]] util::Array3D<Tile>& tiles() noexcept {
			return tiles_;
		}

		[[nodiscard]] const util::Array3D<Tile>& tiles() const noexcept {
			return tiles_;
		}

		/// Add Actor to list
		void addActor(std::shared_ptr<Actor> actor) {
			actors_.push_back(std::move(actor));
			pushActor();
		}

		/// Remove all actors
		void clearActors() {
			actors_.clear();
		}

		[[nodiscard]] std::span<const std::shared_ptr<Actor>> actors() const {
			return actors_;
		}

		/// @brief Gets Actor at given position if it exist
		/// @warning May return nullptr
		[[nodiscard]] std::shared_ptr<Actor> actorAt(sf::Vector3i position);

		/// @brief Gets Actor at given position if it exist
		/// @warning May return nullptr
		[[nodiscard]] std::shared_ptr<const Actor> actorAt(sf::Vector3i position) const;


		/// @brief Gets Actor at given position if it exist
		/// @warning May return nullptr
		[[nodiscard]] std::shared_ptr<Actor> actorAt(core::Position<int> position) {
			return actorAt(util::make3D(position.xy(), position.z));
		}

		/// @brief Gets Actor at given position if it exist
		/// @warning May return nullptr
		[[nodiscard]] std::shared_ptr<const Actor> actorAt(core::Position<int> position) const {
			return actorAt(util::make3D(position.xy(), position.z));
		}

		/// Updates actors until one of them decides to wait input
		void update();

		/// Tile isPassable and have no Actors on it
		[[nodiscard]] bool isFree(sf::Vector3i position) const {
			return isPassable(tiles()[position]) && !actorAt(position);
		}

		void makeSound(Sound sound);

		/// @brief Random tile position
		/// @details position distribution is uniform and independent for both dimensions
		[[nodiscard]] sf::Vector3i randomPositionAt(int level) const {
			return { std::uniform_int_distribution{ 0, tiles().shape().x - 1}(*randomEngine),
					 std::uniform_int_distribution{ 0, tiles().shape().y - 1}(*randomEngine),
					 level };
		}

		/// @brief 3D position of random tile satisfying pred
		/// @details randomPosition(engine) distrbution filtered by pred(tile)
		template <typename Pred>
			requires std::convertible_to<std::invoke_result_t<Pred, Tile>, bool>
		[[nodiscard]] sf::Vector3i randomPositionAt(int level, Pred&& pred) const {
			return randomPositionAt(level, [&pred](const World& world, sf::Vector3i pos) {
				return std::invoke(pred, world.tiles()[pos]);
			});
		}


		/// @brief Random tile position (3D) at given level satisfying pred
		/// @details randomPosition(engine) distrbution filtered by pred(pos, *this)
		/// @param tries Max amount of positions to check before returning nullopt
		template <typename Pred>
			requires std::convertible_to<std::invoke_result_t<Pred, Tile>, bool>
		[[nodiscard]] std::optional<sf::Vector3i> randomPositionAt(int level, int tries, Pred&& pred) const {
			return randomPositionAt(level, tries, [&pred](const World& world, sf::Vector3i pos) {
				return std::invoke(pred, world.tiles()[pos]);
			});
		}

		/// @brief Random tile position (3D) at given level satisfying pred
		/// @details randomPosition(engine) distrbution filtered by pred(pos, *this)
		template <typename Pred>
			requires std::convertible_to<std::invoke_result_t<Pred, const World&, sf::Vector3i>, bool>
		[[nodiscard]] sf::Vector3i randomPositionAt(int level, Pred&& pred) const {
			sf::Vector3i pos;
			do {
				pos = randomPositionAt(level);
			} while (!std::invoke(pred, *this, pos));
			return pos;
		}

		/// @brief Random tile position (3D) at given level satisfying pred
		/// @details randomPosition(engine) distrbution filtered by pred(pos, *this)
		/// @param tries Max amount of positions to check before returning nullopt
		template <typename Pred>
			requires std::convertible_to<std::invoke_result_t<Pred, const World&, sf::Vector3i>, bool>
		[[nodiscard]] std::optional<sf::Vector3i> randomPositionAt(int level, int tries, Pred&& pred) const {
			for (int i = 0; i < tries; ++i) {
				sf::Vector3i pos = randomPositionAt(level);
				if (std::invoke(pred, *this, pos))
					return pos;
			}
			return std::nullopt;
		}

		/// Returns at(position) destination if it's Tile::UP_STAIRS
		[[nodiscard]] std::optional<sf::Vector3i> upStairs(sf::Vector3i position) const {
			return util::getOptional(upStairs_, position);
		}

		/// Returns at(position) destination if it's Tile::DOWN_STAIRS
		[[nodiscard]] std::optional<sf::Vector3i> downStairs(sf::Vector3i position) const {
			return util::getOptional(downStairs_, position);
		}

		/// @brief Create stairs between position1 and position2
		/// @details Sets tiles and registers stairs in maps.
		/// Up or down stairs are choosen automatically by z coordinate.
		void addStairs(sf::Vector3i position1, sf::Vector3i position2);

		void generateStairs();

		/// Add bsp area. Used for debug area rendering
		void addArea(sf::IntRect area, int level) {
			areas_.resize(tiles().shape().z);
			areas_[level].push_back(area);
		}

		/// Get all bsp areas. Used for debug area rendering
		[[nodiscard]] std::span<const sf::IntRect> areas(int level) const noexcept {
			return areas_[level];
		}

		void player(std::shared_ptr<Actor> newPlayer) noexcept {
			player_ = std::move(newPlayer);
		}

		const Actor& player() const noexcept {
			return *player_;
		}

		Actor& player() noexcept {
			return *player_;
		}

		void parseTiles(std::string_view s);
		std::string stringifyTiles() const;
	private:
		util::Array3D<Tile> tiles_;

		util::UnorderedMap<sf::Vector3i, sf::Vector3i> upStairs_;
		util::UnorderedMap<sf::Vector3i, sf::Vector3i> downStairs_;

		std::vector<std::vector<sf::IntRect>> areas_;

		std::vector<std::shared_ptr<Actor>> actors_;
		std::shared_ptr<Actor> player_;

		util::RandomEngine* randomEngine = nullptr;

		void pushActor();
		void popActor();
	};
}

#endif