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

#ifndef LOG_HPP_
#define LOG_HPP_

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <boost/di.hpp>

#include <filesystem>
#include <ranges>

using ConsoleSink = spdlog::sinks::stdout_color_sink_st;
using FileSink = spdlog::sinks::basic_file_sink_st;

auto logModule() {
    return boost::di::make_injector(
        boost::di::bind<FileSink>.to(std::make_shared<FileSink>("log.txt", true)),
        boost::di::bind<spdlog::sinks::sink*[]>.to<ConsoleSink, FileSink>()
    );
}

[[nodiscard]] auto createLogger(std::string name, 
        const std::ranges::range auto& sinks) noexcept {
    return std::make_shared<spdlog::logger>(std::move(name), 
        std::ranges::begin(sinks), std::ranges::end(sinks));
}

[[nodiscard]] auto createLogger(std::string name, auto& injector) noexcept {
    return createLogger(std::move(name), 
        injector.create<std::vector<spdlog::sink_ptr>>());
}

#endif
