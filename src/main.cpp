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

#include "Window.hpp"
#include "log.hpp"
#include "Exception.hpp"

#include <boost/di.hpp>
namespace di = boost::di;

#include <cstdlib>

int main() {
    auto logger = createLogger("main", "log.txt");   
    try {
        logger->info("Loading...");

        auto injector = di::make_injector(
            di::bind<sf::VideoMode>.to(sf::VideoMode::getDesktopMode()),
            di::bind<Title>.to(Title{"The Rune of the Eldest"}),
            di::bind<Style>.to(Style{sf::Style::Fullscreen})
        );
        auto window = injector.create<std::shared_ptr<Window>>();

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
