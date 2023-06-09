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

#include "core/Tile.hpp"
#include "core/AiState.hpp"

#include "util/log.hpp"
#include "util/Exception.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Font.hpp>

#include <filesystem>
#include <unordered_map>

namespace render {
	/// Loads and manages textures
	class AssetManager {
	public:
		/// Error while loading some asset
		class LoadError : public util::RuntimeError {
			using util::RuntimeError::RuntimeError;
		};

		/// Error while loading texture
		class TextureLoadError : public LoadError {
			using LoadError::LoadError;
		};

		/// Error while loading font
		class FontLoadError : public LoadError {
			using LoadError::LoadError;
		};

		/// @brief creates AssetManager and loads textures
		/// @throws AssetManager::TextureLoadError
		AssetManager(util::LoggerFactory& loggerFactory);

		/// @brief Gets texture from given file
		/// @details Loads texture from given file and caches it
		[[nodiscard]] const sf::Texture& texture(const std::filesystem::path& path) const noexcept;

		/// Gets texture for given tile
		[[nodiscard]] const sf::Texture& tileTexture(Tile tile) const noexcept {
			return tileTextures[static_cast<int>(tile)];
		}

		/// Gets player texture
		[[nodiscard]] const sf::Texture& playerTexture() const noexcept {
			return playerTexture_;
		}

		/// Gets icon for given AiState
		[[nodiscard]] const sf::Texture& aiStateIcon(AiState state) const noexcept {
			return aiStateIcons[static_cast<int>(state)];
		}

		/// Gets default font
		[[nodiscard]] const sf::Font& font() const noexcept {
			return font_;
		}

		/// Gets size of single tile
		[[nodiscard]] sf::Vector2i tileSize() const noexcept {
			return tileSize_;
		}
	private:
		mutable std::unordered_map<std::filesystem::path, sf::Texture> textureCache;

		inline const static sf::Vector2i tileSize_{ 16, 16 };
		std::array<sf::Texture, totalTiles> tileTextures;

		sf::Texture playerTexture_;

		std::array<sf::Texture, totalAiStates> aiStateIcons;

		sf::Font font_;

		std::shared_ptr<spdlog::logger> logger;

		[[nodiscard]] sf::Texture& tileTextureMut(Tile tile) noexcept {
			return tileTextures[static_cast<int>(tile)];
		}

		[[nodiscard]] sf::Texture& aiStateIconMut(AiState state) noexcept {
			return aiStateIcons[static_cast<int>(state)];
		}

		void fillTexture(sf::Texture& texture,
			sf::Vector2i size, sf::Color color) const noexcept {
			sf::Image image;
			image.create(size.x, size.y, color);
			texture.loadFromImage(image);
		}

		void loadTexture(sf::Texture& texture, std::string_view logMessage,
			const std::filesystem::path& file) const;
	};
}

#endif