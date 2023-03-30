#include "Game.hpp"

bool wasKeyPressed(sf::Event event, sf::Keyboard::Key key) noexcept {
    return event.type == sf::Event::KeyPressed && event.key.code == key;
}

void Game::run() {
	while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event))
            handleEvent(event);

        render();
    }
}

void Game::handleEvent(sf::Event event) {
    if (event.type == sf::Event::Closed 
     || wasKeyPressed(event, sf::Keyboard::Escape))
        window->close();
}

void Game::render() {
    window->clear(sf::Color::Black);
    window->display();
}
