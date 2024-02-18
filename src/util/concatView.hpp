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

#ifndef CONCAT_VIEW_HPP_
#define CONCAT_VIEW_HPP_

#include <algorithm>
#include <ranges>
#include <iterator>
#include <concepts>
#include <functional>

namespace util {
    namespace detail {
        bool eqRange(std::ranges::range auto range1, std::ranges::range auto range2) {
            if (range1.empty() && range2.empty()) {
                return true;
            } else if (range1.empty() || range2.empty()) {
                return false;
            } else {
                return range1.begin() == range2.begin() && range1.end() == range2.end();
            }
        }
    }

    template <std::ranges::forward_range Range1, std::ranges::forward_range Range2>
        requires std::convertible_to<std::ranges::range_value_t<Range2>, std::ranges::range_value_t<Range1>>
    class concatView : public std::ranges::view_interface<concatView<Range1, Range2>> {
    public:
        using difference_type = ptrdiff_t;
        using value_type = std::ranges::range_value_t<Range1>;

        concatView(Range1&& range1_, Range2&& range2_) : range1{range1_}, range2{range2_} {}
        concatView() = default;

        auto begin() const {
            return *this;
        }

        auto end() const {
            return std::default_sentinel;
        }

        auto& operator++ () {
            if (!std::ranges::empty(range1)) {
                range1 = {std::next(range1.begin()), range1.end()};
            } else {
                range2 = {std::next(range2.begin()), range2.end()};
            }
            return *this;
        }

        auto operator++ (int) {
            auto old = *this;
            ++(*this);
            return old;
        }

        const value_type& operator* () const {
            if (!std::ranges::empty(range1)) {
                return *range1.begin();
            } else {
                return *range2.begin();
            }
        }

        friend bool operator == (concatView<Range1, Range2> lhs, concatView<Range1, Range2> rhs) {
            return detail::eqRange(lhs.range1, rhs.range1) && detail::eqRange(lhs.range2, rhs.range2);
        }

        friend bool operator == (concatView<Range1, Range2> lhs, std::default_sentinel_t) {
            return lhs.range1.empty() && lhs.range2.empty();
        }

        friend bool operator == (std::default_sentinel_t, concatView<Range1, Range2> lhs) {
            return lhs == std::default_sentinel;
        }
    private:
        std::ranges::subrange<std::ranges::iterator_t<Range1>, std::ranges::sentinel_t<Range1>> range1;
        std::ranges::subrange<std::ranges::iterator_t<Range2>, std::ranges::sentinel_t<Range2>> range2;
    };
}

template <std::ranges::borrowed_range Range1, std::ranges::borrowed_range Range2>
inline constexpr bool std::ranges::enable_borrowed_range<util::concatView<Range1, Range2>> = false;

#endif