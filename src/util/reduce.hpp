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

#ifndef REDUCE_HPP_
#define REDUCE_HPP_

#include <algorithm>
#include <numeric>
#include <ranges>
#include <concepts>
#include <functional>

namespace util {
    template <typename T, typename Operand>
    concept BinaryOp = std::convertible_to<std::invoke_result_t<T, Operand, Operand>, Operand>;

    template <typename T, typename Range, typename Result>
    concept ProjectsRange = std::convertible_to<std::invoke_result_t<T, std::ranges::range_value_t<Range>>, Result>;

    template <typename T, std::ranges::forward_range Range,
              BinaryOp<T> Op = std::plus<>> 
        requires std::same_as<std::ranges::range_value_t<Range>, T>
    T reduce(const Range& range, T init = T{}, Op op = Op{}) {
        return std::reduce(range.begin(), range.end(), init, op);
    }

    template <std::ranges::forward_range Range, typename U, 
              BinaryOp<U> Op, ProjectsRange<Range, U> Proj>
    U reduce(const Range& range, U init = U{}, Op op = Op{}, Proj proj = Proj{}) {
        return reduce(std::views::transform(range, proj), init, op);
    }
}

#endif