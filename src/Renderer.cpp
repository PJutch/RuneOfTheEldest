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

#include "Renderer.hpp"

#include "Exception.hpp"

class TextureLoadError : public RuntimeError {
    using RuntimeError::RuntimeError;
};

Renderer::Renderer(std::shared_ptr<Camera> camera,
                   std::shared_ptr<sf::RenderWindow> window_,
                   std::shared_ptr<World> world_, std::shared_ptr<Player> player_, 
                   LoggerFactory& loggerFactory) :
        camera{std::move(camera)},
        world{ std::move(world_) }, player{ std::move(player_) },
        window{ std::move(window_) }, 
        assetLogger{ loggerFactory.create("assets") }  {
    loadTexture(tileTexture(Level::Tile::EMPTY ), "floor tile" , "resources/floor.png" );
    loadTexture(tileTexture(Level::Tile::WALL  ), "wall tile"  , "resources/wall.png"  );
    loadTexture(tileTexture(Level::Tile::UNSEEN), "unseen tile", "resources/unseen.png");

    loadTexture(playerTexture, "player", "resources/player.png");
    
    assetLogger->info("Creating debug tile textures...");
    fillTexture(tileTexture(Level::Tile::ROOM         ), tileSize, sf::Color::Red    );
    fillTexture(tileTexture(Level::Tile::ROOM_ENTRANCE), tileSize, sf::Color::Magenta);
    fillTexture(tileTexture(Level::Tile::PASSAGE      ), tileSize, sf::Color::Blue   );
}

void Renderer::loadTexture(sf::Texture& texture, std::string_view name, const std::filesystem::path& path) const {
    assetLogger->info("Loading {} texture...", name);
    if (!texture.loadFromFile(path.generic_string()))
        throw TextureLoadError{ std::format("Unable to load {} texture", name) };
}

void Renderer::draw(Level& level) {
    window->setView(camera->view());

    for (int x = 0; x < level.shape().x; ++ x)
        for (int y = 0; y < level.shape().y; ++ y)
            draw(level.at(x, y), {x, y});
    
    if (renderAreas_) drawAreas(level);
}

void Renderer::drawAreas(Level& level) {
    for (sf::IntRect area : level.areas())
        drawInWorldRect(area, sf::Color::Transparent, 
                                sf::Color::Green, 1.0);
}

void Renderer::drawPlayer() {
    if (camera->level() != player->level()) 
        return;

    window->setView(camera->view());

    sf::Sprite playerSprite;
    playerSprite.setTexture(playerTexture);
    playerSprite.setPosition(toScreen(player->position()));

    window->draw(playerSprite);
}

void Renderer::draw(Level::Tile tile, sf::Vector2i position) {
    sf::Sprite tileSprite;
    tileSprite.setTexture(tileTexture(tile));
    tileSprite.setPosition(toScreen(position));

    window->draw(tileSprite);
}

void Renderer::drawInWorldRect(sf::IntRect rect, 
        sf::Color fillColor, sf::Color outlineColor, float outlineThickness) {
    sf::RectangleShape rectShape{toScreen({rect.width, rect.height})};
    rectShape.setPosition(toScreen({rect.left, rect.top}));

    rectShape.setFillColor(fillColor);
    rectShape.setOutlineColor(outlineColor);
    rectShape.setOutlineThickness(outlineThickness);

    window->draw(rectShape);
}

void Renderer::draw() {
    window->clear(sf::Color::Black);
    drawWorld();
    drawPlayer();
    window->display();
}

