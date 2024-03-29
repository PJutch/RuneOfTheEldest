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

#ifndef ASSERT_HPP_
#define ASSERT_HPP_

/// @file assert.hpp Assertion based on tracable exceptions

#include "Exception.hpp"

#include <format>
#include <string_view>

namespace util {
    /// If true assertions are enabled
    const bool enableAssertions = false;

    /// @brief Generates message for assertion failed
    /// @param condition Stringified condition
    /// @param message   User provided message
    /// @returns Formatted message
    inline std::string assertion_message(std::string_view condition, std::string_view message = "") {
        if (message.empty())
            return std::format("Assertion failed: {}", condition);
        else
            return std::format("Assertion failed: {} ({})", message, condition);
    }

    /// Exception thrown if assertion failed
    class AssertionFailed : public LogicError {
    public:
        AssertionFailed(std::string_view condition, std::string_view message = "", Stacktrace stacktace_ = {}) :
            LogicError{ assertion_message(condition, message), std::move(stacktace_) } {}
    };
}

/// @brief Assertion macro
/// @warning Always runs condition even in release build
/// @param condition Condition to check
/// @param ... User provided message
/// @throws AssertionFailed if condition evaluates to false
#define TROTE_ASSERT(condition, ...) do { \
    if (::util::enableAssertions) \
        if (!(condition)) \
            throw ::util::AssertionFailed(#condition __VA_OPT__(,) __VA_ARGS__); \
} while (false);

#endif