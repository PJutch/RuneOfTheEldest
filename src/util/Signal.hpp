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

#ifndef SIGNAL_HPP_
#define SIGNAL_HPP_

#include <vector>
#include <functional>
#include <utility>

namespace util {
    template <typename... Args>
    class Signal {
    public:
        void addListener(std::function<void(Args...)> listener) {
            listeners.push_back(std::move(listener));
        }

        template <typename... Args>
        void operator() (Args&&... args) {
            for (auto& listener : listeners)
                listener(std::forward<Args>(args)...);
        }

        template <typename... Args>
        void operator() (Args&&... args) const {
            for (const auto& listener : listeners)
                listener(std::forward<Args>(args)...);
        }
    private:
        std::vector<std::function<void(Args...)>> listeners;
    };
}

#endif