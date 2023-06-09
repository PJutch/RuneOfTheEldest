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

#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_

/// @file Exception.hpp Tracable exception classes and utilities

#define BOOST_STACKTRACE_LINK
#include <boost/stacktrace.hpp>

#include <format>
#include <exception>
#include <stdexcept>

namespace util {
    /// short typedef for stacktrace
    using Stacktrace = boost::stacktrace::stacktrace;

    /// Base class for exceptions with stacktrace support
    class TracableException {
    public:
        /// Location of the error
        [[nodiscard]] const Stacktrace& stacktrace() const noexcept {
            return stacktrace_;
        }

        /// Error description
        [[nodiscard]] virtual const char* what() const noexcept = 0;
    protected:
        TracableException(Stacktrace currentStacktrace) noexcept :
            stacktrace_{ std::move(currentStacktrace) } {}
    private:
        Stacktrace stacktrace_;
    };

    /// Tracable std::runtime_error
    class RuntimeError : public std::runtime_error, public TracableException {
    public:
        RuntimeError(const std::string& message,
            Stacktrace currentStacktrace = {}) noexcept :
            std::runtime_error{ message },
            TracableException{ std::move(currentStacktrace) } {}

        /// Uses std::runtime_error::what as TracableException::what
        [[nodiscard]] const char* what() const noexcept override {
            return std::runtime_error::what();
        }
    };

    /// Tracable std::logic_error
    class LogicError : public std::logic_error, public TracableException {
    public:
        LogicError(const std::string& message,
            Stacktrace currentStacktrace = {}) noexcept :
            std::logic_error{ message },
            TracableException{ std::move(currentStacktrace) } {}

        /// Uses std::logic_error::what as TracableException::what
        [[nodiscard]] const char* what() const noexcept override {
            return std::logic_error::what();
        }
    };
}

/// std::format support of boost::stacktrace::to_string
template <>
struct std::formatter<util::Stacktrace> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const util::Stacktrace& stacktrace, std::format_context& ctx) const {
        return std::ranges::copy(to_string(stacktrace), ctx.out()).out;
    }
};

#endif