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

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <filesystem>

[[nodiscard]] auto createLogger(std::string name, 
                                const std::filesystem::path& file) noexcept {
    using ConsoleSink = spdlog::sinks::stdout_color_sink_st;
    using FileSink = spdlog::sinks::basic_file_sink_st;

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<ConsoleSink>());
    sinks.push_back(std::make_shared<FileSink>(file.string(), true));
    return std::make_shared<spdlog::logger>(std::move(name), 
                                            begin(sinks), end(sinks));
}
