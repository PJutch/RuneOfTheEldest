#include "Game.hpp"

void Game::run() {
	while (window->isOpen()) {
        sf::Event event;
        while (window->pollEvent(event))
            if (event.type == sf::Event::Closed 
                || event.type == sf::Event::KeyPressed 
                && event.key.code == sf::Keyboard::Escape)
                window->close();

        window->clear(sf::Color::Black);
        window->display();
    }
}
