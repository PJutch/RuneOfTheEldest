#include "Game.hpp"

Game::Game(std::shared_ptr<sf::RenderWindow> window_, 
           std::shared_ptr<World> newWorld,
           std::unique_ptr<Renderer> newRenderer) : 
    window{std::move(window_)}, 
    world_{std::move(newWorld)},
    renderer_{std::move(newRenderer)} {} 

bool wasKeyPressed(sf::Event event, sf::Keyboard::Key key) noexcept {
    return event.type == sf::Event::KeyPressed && event.key.code == key;
}

void Game::run() {
    world().generate();

    sf::Clock clock;
	while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event))
            handleEvent(event);

        sf::Time elapsedTime = clock.restart();
        renderer().update(elapsedTime);

        renderer().draw();
    }
}

void Game::handleEvent(sf::Event event) {
    if (event.type == sf::Event::Closed 
     || wasKeyPressed(event, sf::Keyboard::Escape))
        window->close();
}
