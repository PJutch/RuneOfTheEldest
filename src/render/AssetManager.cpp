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

#include "AssetManager.hpp"

#include <string_view>
#include <filesystem>

namespace render {
    AssetManager::AssetManager(LoggerFactory& loggerFactory) : logger{ loggerFactory.create("assets") } {
        logger->info("Loading...");

        loadTexture(tileTextureMut(Tile::EMPTY), "floor tile", "resources/textures/Tile/floor.png");
        loadTexture(tileTextureMut(Tile::WALL), "wall tile", "resources/textures/Tile/wall.png");

        loadTexture(tileTextureMut(Tile::UP_STAIRS), "up stairs tile", "resources/textures/Tile/up_stairs.png");
        loadTexture(tileTextureMut(Tile::DOWN_STAIRS), "down stairs tile", "resources/textures/Tile/down_stairs.png");

        loadTexture(playerTexture_, "player", "resources/textures/player.png");

        loadTexture(goblinTexture_, "goblin", "resources/textures/goblin.png");

        logger->info("Creating debug tile textures...");
        fillTexture(tileTextureMut(Tile::ROOM), tileSize(), sf::Color::Red);
        fillTexture(tileTextureMut(Tile::ROOM_ENTRANCE), tileSize(), sf::Color::Magenta);
        fillTexture(tileTextureMut(Tile::PASSAGE), tileSize(), sf::Color::Blue);

        loadTexture(aiStateIconMut(AiState::INACTIVE), "incative AI state", "resources/textures/AiState/sleeping.png");
        loadTexture(aiStateIconMut(AiState::SEEKING), "seeking AI state", "resources/textures/AiState/confused.png");
        loadTexture(aiStateIconMut(AiState::ATTACKING), "attacking AI state", "resources/textures/AiState/angry.png");

        logger->info("Loading font...");
        if (!font_.loadFromFile("resources/fonts/Roboto/Roboto-Medium.ttf"))
            throw FontLoadError{ "Unable to load font" };

        logger->info("Finished loading...");
    }

    void AssetManager::loadTexture(sf::Texture& texture, std::string_view name, const std::filesystem::path& path) const {
        logger->info("Loading {} texture...", name);
        if (!texture.loadFromFile(path.generic_string()))
            throw TextureLoadError{ std::format("Unable to load {} texture", name) };
    }
}
