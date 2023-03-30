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

#include "log.hpp"
#include "Exception.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <boost/di.hpp>

#include <cstdlib>

sf::String createSfString(std::string_view string) {
    return sf::String::fromUtf8(std::ranges::begin(string), 
                                std::ranges::end(string));
}

int main() {
    auto logInjector = logModule();
    auto logger = createLogger("main", logInjector);   

    try {
        logger->info("Loading...");

        auto injector = boost::di::make_injector(
            std::move(logInjector),
            boost::di::bind<sf::VideoMode>.to(sf::VideoMode::getDesktopMode()),
            boost::di::bind<sf::RenderWindow>.to([](const auto& injector) {
                return std::make_shared<sf::RenderWindow>(
                    injector.template create<sf::VideoMode>(),
                    createSfString("The Rune of the Eldest"), 
                    sf::Style::Fullscreen
                );
            })
        );
        auto window = injector.create<std::shared_ptr<sf::RenderWindow>>();

        logger->info("Loading complete");

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

        logger->info("Exiting...");
    } catch (const TracableException& e) {
        logger->critical("Error occured: {}\nStacktrace:\n{}", 
                         e.what(), e.stacktrace());
    } catch (const std::exception& e) {
        logger->critical("Error occured: {}", e.what());
    }

    logger->info("Exited");

    return EXIT_SUCCESS;
}
