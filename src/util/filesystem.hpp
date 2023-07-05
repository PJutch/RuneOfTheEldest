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

#ifndef FILESYSTEM_HPP_
#define FILESYSTEM_HPP_

#include <filesystem>
#include <fstream>

namespace util {
	using DirIter = std::filesystem::directory_iterator;

	template <typename Callback> requires std::invocable<Callback, const std::filesystem::directory_entry&>
	void forEachEntry(const std::filesystem::path& path, Callback&& callback) {
		for (std::filesystem::directory_iterator iter{ path }; iter != std::filesystem::directory_iterator{}; ++iter)
			callback(*iter);
	}

	template <typename Callback> requires std::invocable<Callback, const std::filesystem::directory_entry&>
	void forEachFile(const std::filesystem::path& path, Callback&& callback) {
		forEachEntry(path, [&callback](const std::filesystem::directory_entry& entry) {
			if (entry.is_regular_file())
				callback(entry);
		});
	}

	template <typename Callback> requires std::invocable<Callback, std::ifstream&>
	void forEachFile(const std::filesystem::path& path, Callback&& callback) {
		forEachFile(path, [&callback](const std::filesystem::path& filePath) {
			std::ifstream file{ filePath };
			callback(file);
		});
	}
}

#endif