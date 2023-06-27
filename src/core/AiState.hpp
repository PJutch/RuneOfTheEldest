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

/// State of Enemy AI
enum class AiState {
	NONE,       ///< Controlled by player
	INACTIVE,   ///< Hasn't seen player before
	SEEKING,    ///< Has seen player but can't see it currently
	ATTACKING,  ///< Can see player
	TOTAL_      ///< Technical enumerator. Should always be last
};

const int totalAiStates = static_cast<int>(AiState::TOTAL_);

#endif