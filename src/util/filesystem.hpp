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
	/// Shortcut for directory_iterator
	using DirIter = std::filesystem::directory_iterator;
	/// Shortcut for directory_entry
	using DirEntry = std::filesystem::directory_entry;

	/// Calls callback with each directory in given directory
	template <typename Callback> requires std::invocable<Callback, const DirEntry&>
	void forEachEntry(const std::filesystem::path& path, Callback&& callback) {
		for (DirIter iter{ path }; iter != DirIter{}; ++iter)
			callback(*iter);
	}

	/// Calls callback with each file entry in given directory
	template <typename Callback> requires std::invocable<Callback, const DirEntry&>
	void forEachFile(const std::filesystem::path& path, Callback&& callback) {
		forEachEntry(path, [&callback](const DirEntry& entry) {
			if (entry.is_regular_file())
				callback(entry);
		});
	}

	/// Calls callback with each file stream in given directory
	template <typename Callback> requires std::invocable<Callback, std::ifstream&>
	void forEachFile(const std::filesystem::path& path, Callback&& callback) {
		forEachFile(path, [&callback](const std::filesystem::path& filePath) {
			std::ifstream file{ filePath };
			callback(file);
		});
	}

	/// Calls callback with each file stream and path in given directory
	template <typename Callback> requires std::invocable<Callback, std::ifstream&, const std::filesystem::path&>
	void forEachFile(const std::filesystem::path& path, Callback&& callback) {
		forEachFile(path, [&callback](const std::filesystem::path& filePath) {
			std::ifstream file{filePath};
			callback(file, filePath);
		});
	}

	/// Reads all contents of a stream
	inline std::string readWhole(std::istream& is) {
		return {std::istreambuf_iterator{is}, std::istreambuf_iterator<char>{}};
	}

	/// Reads all contents of a file if it exists
	inline std::optional<std::string> readWhole(const std::filesystem::path& path) {
		if (std::ifstream file{path}) {
			return readWhole(file);
		} else {
			return std::nullopt;
		}
	}

	/// Replaces file contents with a string
	inline bool writeWhole(const std::filesystem::path& path, std::string_view s) {
		if (std::ofstream file{path}) {
			file << s;
			return true;
		} else {
			return false;
		}
	}

	/// Makes dot-separated identifier from path raltive to base path
	inline std::string toIdentifier(const std::filesystem::path& path, const std::filesystem::path& base) {
		auto result = std::filesystem::relative(path, base).replace_extension().generic_string();
		for (char& c : result) {
			if (c == '/') {
				c = '.';
			}
		}
		return result;
	}
	
}

#endif