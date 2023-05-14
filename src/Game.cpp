#include "Game.hpp"

Game::Game(std::shared_ptr<sf::RenderWindow> window_, 
           std::shared_ptr<Level> level_, 
           std::unique_ptr<Renderer> newRenderer,
            DungeonGenerator newDungeonGenerator,
           LoggerFactory& loggerFactory) : 
    window{std::move(window_)}, 
    level{std::move(level_)}, 
    renderer_{std::move(newRenderer)},
    dungeonGenerator_{std::move(newDungeonGenerator)},
    generationLogger{loggerFactory.create("generation")} {} 

void Game::generateLevel() {
    generationLogger->info("Started");
    level->generateBlank({50, 50});

    generationLogger->info("Generating dungeon...");
    dungeonGenerator()();

    generationLogger->info("Generating walls...");
    level->generateWalls();

    generationLogger->info("Finished");
}

bool wasKeyPressed(sf::Event event, sf::Keyboard::Key key) noexcept {
    return event.type == sf::Event::KeyPressed && event.key.code == key;
}

void Game::run() {
    generateLevel();

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
