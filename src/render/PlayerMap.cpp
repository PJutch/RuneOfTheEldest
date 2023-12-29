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

#include "PlayerMap.hpp"

#include "AssetManager.hpp"

#include "core/World.hpp"

#include "util/raycast.hpp"
#include "util/Direction.hpp"
#include "util/parse.hpp"
#include "util/parseKeyValue.hpp"
#include "util/stringify.hpp"

namespace render {
	PlayerMap::PlayerMap(std::shared_ptr<core::World> world_, std::shared_ptr<AssetManager> assets_, 
						 std::shared_ptr<util::Raycaster> raycaster_) :
		world{ std::move(world_) }, assets{std::move(assets_)}, raycaster{std::move(raycaster_)} {}

	const bool seeEverything = false;

	void PlayerMap::onGenerate() {
		seenActors_.clear();
		clearSounds();

		tileStates.assign(world->tiles().shape(), seeEverything ? TileState::VISIBLE : TileState::UNSEEN);
	}

	bool PlayerMap::canSee(sf::Vector3i position) const noexcept {
		return seeEverything || raycaster->canSee(world->player().position(), position);
	}

	void PlayerMap::updateTiles() {
		if (seeEverything)
			return;

		auto [shapeX, shapeY, shapeZ] = world->tiles().shape();
		for (int z = 0; z < shapeZ; ++z) {
			for (int x = 0; x < shapeX; ++x)
				for (int y = 0; y < shapeY; ++y)
					if (raycaster->canSee(world->player().position(), { x, y, z }))
						tileStates[{ x, y, z }] = TileState::VISIBLE;
					else if (tileState({ x, y, z }) == TileState::VISIBLE)
						tileStates[{ x, y, z }] = TileState::MEMORIZED;
		}
	}

	void PlayerMap::updateActors() {
		std::erase_if(seenActors_, [this, &player = world->player(), &world = *world](auto actor) -> bool {
			return seeEverything || raycaster->canSee(player.position(), actor.position);
		});

		for (const auto& actor : world->actors())
			if (actor->isAlive() && (seeEverything || raycaster->canSee(world->player().position(), actor->position())))
				seenActors_.emplace_back(actor->position(), 
										 actor->hp(), actor->maxHp(), 
										 actor->mana(), actor->maxMana(),
					                     actor->controller().aiState(), actor->texture());

		std::ranges::sort(seenActors_, {}, [](SeenActor actor) {
			return actor.position.y;
		});
	}

	void PlayerMap::discoverLevelTiles(int z) {
		if (seeEverything)
			return;

		auto [shapeX, shapeY, shapeZ] = world->tiles().shape();
		for (int x = 0; x < shapeX; ++x)
			for (int y = 0; y < shapeY; ++y) 
				if (tileStates[{ x, y, z }] == TileState::UNSEEN) {
					if (std::ranges::any_of(util::directions<int>, [&](sf::Vector2i d) {
						sf::Vector3i pos{x + d.x, y + d.y, z};
						return world->tiles().isValidPosition(pos) && world->tiles()[pos] != core::Tile::WALL;
					})) {
						tileStates[{ x, y, z }] = TileState::MEMORIZED;
					}
				}
	}

	void PlayerMap::discoverLevelActors(int z) {
		if (seeEverything)
			return;

		std::erase_if(seenActors_, [z](auto actor) -> bool {
			return actor.position.z == z;
		});

		for (const auto& actor : world->actors())
			if (actor->isAlive() && actor->position().z == z)
				seenActors_.emplace_back(actor->position(),
					actor->hp(), actor->maxHp(),
					actor->mana(), actor->maxMana(),
					actor->controller().aiState(), actor->texture());

		std::ranges::sort(seenActors_, {}, [](SeenActor actor) {
			return actor.position.y;
		});
	}

	namespace {
		class UnknownTileState : public util::RuntimeError {
		public:
			UnknownTileState(char c, util::Stacktrace stacktrace = {}) : 
				util::RuntimeError{std::format("\'{}\' tile state is unknown", c), std::move(stacktrace)} {}
		};
	}

	void PlayerMap::parseTileStates(std::string_view data) {
		auto newTileStates = util::parseCharMap(data).transform([](char c) {
			switch (c) {
			case '.':
				return PlayerMap::TileState::MEMORIZED;
			case '?':
				return PlayerMap::TileState::UNSEEN;
			default:
				throw UnknownTileState{c};
			}
		});

		if (seeEverything) {
			tileStates.assign(newTileStates.shape(), render::PlayerMap::TileState::VISIBLE);
		} else {
			tileStates = newTileStates;
		}
	}

	[[nodiscard]] std::string PlayerMap::stringifyTileStates() const {
		return util::stringifyCharMap(tileStates.transform([](PlayerMap::TileState tileState) {
			switch (tileState) {
			case PlayerMap::TileState::VISIBLE: 
			case PlayerMap::TileState::MEMORIZED:
				return '.';
			case PlayerMap::TileState::UNSEEN: 
				return '?';
			default:
				TROTE_ASSERT(false, "unreachable");
			}
		}));
	}

	void PlayerMap::parseSeenActor(std::string_view data) {
		SeenActor result;

		util::KeyValueVisitor visitor;
		visitor.key("position").unique().required().callback([&](std::string_view data) {
			result.position = util::parseVector3i(data);
		});

		visitor.key("hp").unique().required().callback([&](std::string_view data) {
			result.hp = util::parseReal(data);
		});

		visitor.key("maxHp").unique().required().callback([&](std::string_view data) {
			result.maxHp = util::parseReal(data);
		});

		visitor.key("mana").unique().required().callback([&](std::string_view data) {
			result.mana = util::parseReal(data);
		});

		visitor.key("maxMana").unique().required().callback([&](std::string_view data) {
			result.maxMana = util::parseReal(data);
		});

		visitor.key("aiState").unique().required().callback([&](std::string_view data) {
			result.aiState = parseAiState(data);
		});

		visitor.key("texture").unique().required().callback([&](std::string_view data) {
			result.texture = &assets->texture(data);
		});

		util::forEackKeyValuePair(data, visitor);
		visitor.validate();

		seenActors_.push_back(result);
	}

	[[nodiscard]] std::string PlayerMap::stringifySeenActor(SeenActor actor) const {
		return std::format("position {}\nhp {}\nmaxHp {}\nmana {}\nmaxMana {}\naiState {}\ntexture {}\n",
			               util::stringifyVector3(actor.position), actor.hp, actor.maxHp, actor.mana, actor.maxMana,
			               stringifyAiState(actor.aiState), assets->texturePath(*actor.texture).generic_string());
	}
}
