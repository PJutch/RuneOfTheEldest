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

#ifndef ASSET_MANAGER_HPP_
#define ASSET_MANAGER_HPP_

#include "Tile.hpp"

#include "log.hpp"
#include "Exception.hpp"

#include <SFML/Graphics.hpp>

/// loads and manages textures
class AssetManager {
public:
	/// error whil loading texture
	class TextureLoadError : public RuntimeError {
		using RuntimeError::RuntimeError;
	};

	/// @brief creates AssetManager and loads textures
	/// @throws AssetManager::TextureLoadError
	AssetManager(LoggerFactory& loggerFactory);

	/// Gets texture for given tile
	const sf::Texture& tileTexture(Tile tile) const noexcept {
		return tileTextures[static_cast<int>(tile)];
	}

	/// Gets player texture
	const sf::Texture& playerTexture() const noexcept {
		return playerTexture_;
	}

	/// Gets size of single tile
	sf::Vector2i tileSize() const noexcept {
		return tileSize_;
	}
private:
	inline const static sf::Vector2i tileSize_{ 16, 16 };
	std::array<sf::Texture, totalTiles> tileTextures;

	sf::Texture playerTexture_;

	std::shared_ptr<spdlog::logger> logger;

	sf::Texture& tileTextureMut(Tile tile) noexcept {
		return tileTextures[static_cast<int>(tile)];
	}

	void fillTexture(sf::Texture& texture,
		sf::Vector2i size, sf::Color color) const noexcept {
		sf::Image image;
		image.create(size.x, size.y, color);
		texture.loadFromImage(image);
	}

	void loadTexture(sf::Texture& texture, std::string_view name,
		const std::filesystem::path& file) const;
};

#endif