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

#ifndef MAP_HPP_
#define MAP_HPP_

#include <boost/container_hash/hash.hpp>

#include <unordered_map>
#include <functional>
#include <utility>
#include <concepts>
#include <optional>

namespace util {
    template <typename Map, typename Key>
    [[nodiscard]] typename Map::mapped_type* getPtr(Map& map, const Key& key) {
        typename Map::iterator iter = map.find(key);
        if (iter == map.end())
            return nullptr;

        return &iter->second;
    }

    template <typename Map, typename Key>
    [[nodiscard]] const typename Map::mapped_type* getPtr(const Map& map, const Key& key) {
        typename Map::const_iterator iter = map.find(key);
        if (iter == map.end())
            return nullptr;

        return &iter->second;
    }

    template <typename Map, typename Key>
    [[nodiscard]] std::optional<typename Map::mapped_type> getOptional(const Map& map, const Key& key) {
        typename Map::const_iterator iter = map.find(key);
        if (iter == map.end())
            return std::nullopt;

        return iter->second;
    }

    template <typename Map, typename Key>
    [[nodiscard]] std::optional<typename Map::mapped_type> getAndErase(Map& map, const Key& key) {
        typename Map::const_iterator iter = map.find(key);
        if (iter == map.end())
            return std::nullopt;

        auto value = std::move(iter->second);
        map.erase(iter);
        return value;
    }

    template <typename Key, typename Mapped>
        requires std::invocable<boost::hash<Key>, Key>
    using UnorderedMap = std::unordered_map<Key, Mapped, boost::hash<Key>>;
}

#endif