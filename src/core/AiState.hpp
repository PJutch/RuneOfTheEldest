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

#ifndef AI_STATE_HPP_
#define AI_STATE_HPP_

#include "util/assert.hpp"

#include <JutchsON.hpp>

#include <boost/describe.hpp>

/// State of Enemy AI
BOOST_DEFINE_ENUM(AiState,
	NONE,       ///< Has no AI (may be Player controlled)
	INACTIVE,   ///< Hasn't seen player before
	CHECKING,   ///< Checking suspisious location
	WANDERING,  ///< Wanders randomly
	ATTACKING,  ///< Can see player
	TOTAL_      ///< Technical enumerator. Should always be last
)

const int totalAiStates = static_cast<int>(AiState::TOTAL_);

class UnknownAiState : public util::RuntimeError {
public:
    UnknownAiState(std::string_view s, util::Stacktrace stacktrace = {}) :
        util::RuntimeError{std::format("Unknow ai state \"{}\"", s), std::move(stacktrace)} {}
};

inline AiState parseAiState(std::string_view data) {
    if (data == "none") { 
        return AiState::NONE; 
    } else if (data == "inactive") {
        return AiState::INACTIVE;
    } else if (data == "checking") {
        return AiState::CHECKING;
    } else if (data == "wandering") {
        return AiState::WANDERING;
    } else if (data == "attacking") {
        return AiState::ATTACKING;
    } else {
        throw UnknownAiState{data};
    }
}

inline std::string stringifyAiState(AiState aiState) {
    switch (aiState) {
    case AiState::NONE:
        return "none";
    case AiState::INACTIVE:
        return "inactive";
    case AiState::CHECKING:
        return "checking";
    case AiState::WANDERING:
        return "wandering";
    case AiState::ATTACKING:
        return "attacking";
    default:
        TROTE_ASSERT(false, "unreachable");
    }
}

namespace JutchsON {
    template <>
    struct Parser<AiState> {
        JutchsON::ParseResult<AiState> operator() (JutchsON::StringView s, const auto&, Context) {
            if (s == "none") {
                return AiState::NONE;
            } else if (s == "inactive") {
                return AiState::INACTIVE;
            } else if (s == "checking") {
                return AiState::CHECKING;
            } else if (s == "wandering") {
                return AiState::WANDERING;
            } else if (s == "attacking") {
                return AiState::ATTACKING;
            } else {
                return JutchsON::ParseResult<AiState>::makeError(s.location(), std::format("Unknow ai state \"{}\"", s.asStd()));
            }
        }
    };

    template <>
    struct Writer<AiState> {
        std::string operator() (AiState state, const auto&, Context) {
            return stringifyAiState(state);
        }
    };
}

#endif