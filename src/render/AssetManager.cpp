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

#include "render/draw/Primitives.hpp"

#include "util/parseKeyValue.hpp"
#include "util/filesystem.hpp"

#include <string_view>

namespace render {
    AssetManager::AssetManager(util::LoggerFactory& loggerFactory, util::RandomEngine& randomEngine_) : 
            logger{loggerFactory.create("assets")}, randomEngine{&randomEngine_} {
        logger->info("Loading textures...");

        loadTexture(tileTextureMut(core::Tile::EMPTY), "floor tile texture", "resources/textures/Tiles/floor.png");
        loadTexture(tileTextureMut(core::Tile::WALL), "wall tile texture", "resources/textures/Tiles/wall.png");

        loadTexture(tileTextureMut(core::Tile::UP_STAIRS), "up stairs tile texture", "resources/textures/Tiles/up_stairs.png");
        loadTexture(tileTextureMut(core::Tile::DOWN_STAIRS), "down stairs tile texture", "resources/textures/Tiles/down_stairs.png");

        logger->info("Creating debug tile textures...");
        fillTexture(tileTextureMut(core::Tile::ROOM), tileSize(), sf::Color::Red);
        fillTexture(tileTextureMut(core::Tile::ROOM_ENTRANCE), tileSize(), sf::Color::Magenta);
        fillTexture(tileTextureMut(core::Tile::PASSAGE), tileSize(), sf::Color::Blue);
        fillTexture(tileTextureMut(core::Tile::COMPONENT1), tileSize(), sf::Color::Red);
        fillTexture(tileTextureMut(core::Tile::COMPONENT2), tileSize(), sf::Color::Green);
        fillTexture(tileTextureMut(core::Tile::COMPONENT3), tileSize(), sf::Color::Blue);

        loadTexture(aiStateIconMut(AiState::INACTIVE), "incative AI state icon", "resources/textures/AiStates/sleeping.png");
        loadTexture(aiStateIconMut(AiState::CHECKING), "checking AI state icon", "resources/textures/AiStates/curious.png");
        loadTexture(aiStateIconMut(AiState::WANDERING), "wandering AI state icon", "resources/textures/AiStates/confused.png");
        loadTexture(aiStateIconMut(AiState::ATTACKING), "attacking AI state icon", "resources/textures/AiStates/angry.png");

        loadTexture(soundIconMut(core::Sound::Type::WALK, false  ),    "enemy walk sound icon", "resources/textures/Sounds/walkEnemy.png"   );
        loadTexture(soundIconMut(core::Sound::Type::WALK, true   ),   "friend walk sound icon", "resources/textures/Sounds/walkFriend.png"  );
        loadTexture(soundIconMut(core::Sound::Type::ATTACK, false),  "enemy attack sound icon", "resources/textures/Sounds/attackEnemy.png" );
        loadTexture(soundIconMut(core::Sound::Type::ATTACK, true ), "friend attack sound icon", "resources/textures/Sounds/attackFriend.png");

        logger->info("Loading potion textures...");
        util::forEachFile("resources/textures/Potions", [&](const std::filesystem::path& path) {
            potionTextures.push_back(&texture(path));
        });

        logger->info("Loading font...");
        if (!font_.loadFromFile("resources/fonts/Roboto/Roboto-Medium.ttf"))
            throw FontLoadError{ "Unable to load font" };

        logger->info("Finished loading...");
    }

    [[nodiscard]] const sf::Texture& AssetManager::texture(const std::filesystem::path& path) const noexcept {
        if (auto iter = textureCache.find(path); iter != textureCache.end())
            return iter->second;

        loadTexture(textureCache[path], std::format("texture from {}", path.generic_string()), path);
        return textureCache[path];
    }

    void AssetManager::loadTexture(sf::Texture& texture, std::string_view name, const std::filesystem::path& path) const {
        logger->info("Loading {}...", name);
        if (!texture.loadFromFile(path.generic_string()))
            throw TextureLoadError{ std::format("Unable to load {}", name) };
    }

    [[nodiscard]] const sf::Texture& AssetManager::scrollTexture(const sf::Texture& spellIcon) const {
        sf::RenderTexture& result = scrollTextureCache[&spellIcon];
        if (result.getSize() == sf::Vector2u{0, 0}) {
            const sf::Texture& base = texture("resources/textures/scroll.png");
            result.create(2 * base.getSize().x, 2 * base.getSize().y);

            result.clear(sf::Color::Transparent);
            drawSprite(result, {0, 0}, {0, 0}, base, 1.0, 2.0);
            drawSprite(result, util::geometry_cast<float>(result.getSize()) / 2.f, 
                       util::geometry_cast<float>(spellIcon.getSize()) / 2.f, spellIcon);
            result.display();
        }
        return result.getTexture();
    }

    [[nodiscard]] const sf::Texture& AssetManager::potionTexture(const sf::Texture& base, const sf::Texture& label) const {
        sf::RenderTexture& result = potionTextureCache[{&base, &label}];
        if (result.getSize() == sf::Vector2u{0, 0}) {
            result.create(base.getSize().x, base.getSize().y);

            result.clear(sf::Color::Transparent);
            drawSprite(result, {0, 0}, {0, 0}, base);
            drawSprite(result, {0, 0}, {0, 0}, label);
            result.display();
        }
        return result.getTexture();
    }

    namespace {
        class UnknownTextureType : public util::RuntimeError {
        public:
            UnknownTextureType(std::string_view type, util::Stacktrace stacktrace = {}) :
                util::RuntimeError{std::format("Can't load \"{}\" texture", type), std::move(stacktrace)} {}
        };
    }

    [[nodiscard]] const sf::Texture& AssetManager::parse(std::string_view s) const {
        auto [type, data] = util::parseKeyValuePair(s);
        if (type == "file") {
            return texture(data);
        } else if (type == "scroll") {
            return scrollTexture(parse(data));
        } else if (type == "potion") {
            util::KeyValueVisitor visitor;

            const sf::Texture* base;
            visitor.key("base").unique().required().callback([&](std::string_view data) {
                base = &parse(data);
            });

            const sf::Texture* label;
            visitor.key("label").unique().required().callback([&](std::string_view data) {
                label = &parse(data);
            });

            util::forEackInlineKeyValuePair(data, visitor);
            visitor.validate();

            return potionTexture(*base, *label);
        } else {
            throw UnknownTextureType(type);
        }
    }

    namespace {
        class UnknownTexture : public util::RuntimeError {
        public:
            UnknownTexture(util::Stacktrace stacktrace = {}) :
                util::RuntimeError{"Can't find texture in caches", std::move(stacktrace)} {}
        };
    }

    [[nodiscard]] std::filesystem::path AssetManager::texturePath(const sf::Texture& t) const {
        if (auto iter = std::ranges::find(textureCache, &t, [](const auto& p) { return &p.second; });
            iter != textureCache.end()) {
            return iter->first;
        } else {
            throw UnknownTexture{};
        }
    }

    [[nodiscard]] std::string AssetManager::stringify(const sf::Texture& t) const {
        if (auto iter = std::ranges::find(textureCache, &t, [](const auto& p) { return &p.second; }); 
                iter != textureCache.end()) {
            return std::format("file {}", iter->first.generic_string());
        } else if (auto iter = std::ranges::find(scrollTextureCache, &t, [](const auto& p) { return &p.second.getTexture(); }); 
                iter != scrollTextureCache.end()) {
            return std::format("scroll {}", stringify(*iter->first));
        } else if (auto iter = std::ranges::find(potionTextureCache, &t, [](const auto& p) { return &p.second.getTexture(); });
                iter != potionTextureCache.end()) {
            return std::format("potion base {}, label {}", stringify(*iter->first.first), stringify(*iter->first.second));
        } else {
            throw UnknownTexture{};
        }
    }
}
