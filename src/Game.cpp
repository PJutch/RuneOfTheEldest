#include "Game.hpp"

Game::Game(std::shared_ptr<sf::RenderWindow> window_, 
           std::shared_ptr<Level> level_, 
           std::unique_ptr<Renderer> renderer_,
           DungeonGenerator dungeonGenerator_) : 
        window{std::move(window_)}, 
        level{std::move(level_)}, 
        renderer{std::move(renderer_)},
        dungeonGenerator{std::move(dungeonGenerator_)} {
    generateLevel();
} 

void Game::generateLevel() {
    level->generateBlank({50, 50});
    dungeonGenerator();
    level->generateWalls();
}

bool wasKeyPressed(sf::Event event, sf::Keyboard::Key key) noexcept {
    return event.type == sf::Event::KeyPressed && event.key.code == key;
}

void Game::run() {
    sf::Clock clock;
	while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event))
            handleEvent(event);

        sf::Time elapsedTime = clock.restart();
        renderer->update(elapsedTime);

        renderer->draw();
    }
}

void Game::handleEvent(sf::Event event) {
    if (event.type == sf::Event::Closed 
     || wasKeyPressed(event, sf::Keyboard::Escape))
        window->close();
}
