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

#ifndef HUD_HPP_
#define HUD_HPP_

#include "render/fwd.hpp"
#include "core/fwd.hpp"
#include "core/World.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <optional>

namespace render {
	void drawHud(sf::RenderTarget& target, const AssetManager& assets, 
		         const core::World& world, const core::XpManager& xpManager);

	std::optional<int> clickedSpell(sf::Vector2i clickPos, sf::RenderTarget& target, const core::Actor& player);
	std::optional<int> clickedItem(sf::Vector2i clickPos, sf::RenderTarget& target, const core::Actor& player);
}

#endif