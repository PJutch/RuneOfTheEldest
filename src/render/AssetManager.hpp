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
#include "core/Sound.hpp"

#include "util/log.hpp"
#include "util/Exception.hpp"
#include "util/random.hpp"
#include "util/Map.hpp"
#include "util/parseKeyValue.hpp"

#include <JutchsON.hpp>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Font.hpp>

#include <filesystem>

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
		AssetManager(util::LoggerFactory& loggerFactory, util::RandomEngine& randomEngine);

		/// @brief Gets texture from given file
		/// @details Loads texture from given file and caches it
		[[nodiscard]] const sf::Texture& texture(const std::filesystem::path& path) const noexcept;

		/// Gets texture for given tile
		[[nodiscard]] const sf::Texture& tileTexture(core::Tile tile) const noexcept {
			return tileTextures[static_cast<int>(tile)];
		}

		/// Gets icon for given sound type and source side
		[[nodiscard]] const sf::Texture& soundIcon(core::Sound::Type type, bool isSourceOnPlayerSide) const noexcept {
			return soundIcons[static_cast<ptrdiff_t>(type) * 2 + static_cast<ptrdiff_t>(isSourceOnPlayerSide)];
		}

		/// Gets icon for given AiState
		[[nodiscard]] const sf::Texture& aiStateIcon(AiState state) const noexcept {
			return aiStateIcons[static_cast<int>(state)];
		}

		/// Creates and caches texture for scroll
		[[nodiscard]] const sf::Texture& scrollTexture(const sf::Texture& spellIcon) const;

		/// Creates and caches texture for potion
		[[nodiscard]] const sf::Texture& potionTexture(const sf::Texture& base, const sf::Texture& label) const;

		/// Chooses a random base texture for potion
		[[nodiscard]] const sf::Texture& randomPotionBaseTexture() const {
			return *potionTextures[std::uniform_int_distribution<ptrdiff_t>{0, std::ssize(potionTextures) - 1}(*randomEngine)];
		}

		/// Gets default font
		[[nodiscard]] const sf::Font& font() const noexcept {
			return font_;
		}

		/// Gets size of single tile
		[[nodiscard]] sf::Vector2i tileSize() const noexcept {
			return tileSize_;
		}

		[[nodiscard]] std::filesystem::path texturePath(const sf::Texture& texture) const;

		[[nodiscard]] const sf::Texture& parse(std::string_view data) const;
		[[nodiscard]] std::string stringify(const sf::Texture& texture) const;
	private:
		mutable util::UnorderedMap<std::filesystem::path, sf::Texture> textureCache;

		inline const static sf::Vector2i tileSize_{ 16, 16 };
		std::array<sf::Texture, core::totalTiles> tileTextures;

		std::array<sf::Texture, core::Sound::totalTypes * 2> soundIcons;

		std::array<sf::Texture, totalAiStates> aiStateIcons;

		mutable util::UnorderedMap<const sf::Texture*, sf::RenderTexture> scrollTextureCache;
		mutable util::UnorderedMap<std::pair<const sf::Texture*, const sf::Texture*>, sf::RenderTexture> potionTextureCache;

		std::vector<const sf::Texture*> potionTextures;

		sf::Font font_;

		util::RandomEngine* randomEngine;

		std::shared_ptr<spdlog::logger> logger;

		[[nodiscard]] sf::Texture& tileTextureMut(core::Tile tile) noexcept {
			return tileTextures[static_cast<int>(tile)];
		}

		[[nodiscard]] sf::Texture& aiStateIconMut(AiState state) noexcept {
			return aiStateIcons[static_cast<int>(state)];
		}

		[[nodiscard]] sf::Texture& soundIconMut(core::Sound::Type type, bool isSourceOnPlayerSide) noexcept {
			return soundIcons[static_cast<ptrdiff_t>(type) * 2 + static_cast<ptrdiff_t>(isSourceOnPlayerSide)];
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

namespace JutchsON {
	template <>
	struct Parser<const sf::Texture*> {
		ParseResult<const sf::Texture*> operator() (StringView s, const auto& env, Context context) {
			auto stripped = strip(s);
			if (!stripped.empty() && stripped.front() == '<') {
				return parseVariant(s).then([&](auto pair) -> ParseResult<const sf::Texture*> {
					auto [type, data] = pair;
					if (type == "scroll") {
						return parse<const sf::Texture*>(data, env, context).map([&](auto texture) {
							return &env.assets->scrollTexture(*texture);
						});
					} else if (type == "potion") {
						util::KeyValueVisitor visitor;

						const sf::Texture* base;
						visitor.key("base").unique().required().callback([&](std::string_view data) {
							base = *parse<const sf::Texture*>(data, env, context);
						});

						const sf::Texture* label;
						visitor.key("label").unique().required().callback([&](std::string_view data) {
							label = *parse<const sf::Texture*>(data, env, context);
						});

						util::forEackInlineKeyValuePair(data.asStd(), visitor);
						visitor.validate();

						return &env.assets->potionTexture(*base, *label);
					} else {
						return ParseResult<const sf::Texture*>::makeError(s.location(), std::format("Unknown texture type {}", type.asStd()));
					}
				});
			} else {
				return parse<std::string>(s, env, context).map([&](std::string_view s) {
					return &env.assets->texture(s);
				});
			}
		}
	};

	template <>
	struct Writer<const sf::Texture*> {
		template <typename Env>
		std::string operator() (const sf::Texture* t, Env&& env, Context context) {
			return write(env.assets->stringify(*t), std::forward<Env>(env), context);
		}
	};
}

#endif