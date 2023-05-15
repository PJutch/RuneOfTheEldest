#include "Game.hpp"

Game::Game(std::shared_ptr<World> newWorld,
           std::shared_ptr<sf::RenderWindow> window_,
           std::shared_ptr<Camera> camera_,
           std::unique_ptr<Renderer> newRenderer) :
    world_{std::move(newWorld)},
    window{std::move(window_)},
    camera{std::move(camera_)},
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
        camera->update(elapsedTime);

        renderer().draw();
    }
}

void Game::handleEvent(sf::Event event) {
    if (event.type == sf::Event::Closed
        || wasKeyPressed(event, sf::Keyboard::Escape))
        window->close();
    else
        camera->handleEvent(event);
}
