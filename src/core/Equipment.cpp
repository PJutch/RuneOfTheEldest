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

#include "Equipment.hpp"

#include "Actor.hpp"
#include "ItemManager.hpp"

namespace core {
	void Equipment::identify() {
		items->identify(id());
	}

	[[nodiscard]] const sf::Texture& Equipment::icon() const {
		// return items->isIdentified(id()) ? assets->potionTexture(*icon_, *stats.label) : *icon_;
		return *stats.icon;
	}

	[[nodiscard]] std::string Equipment::name() const {
		return items->isIdentified(id()) ? stats.name : std::format("Unknown {}", util::toLower(util::enumeratorName(stats.slot)));
	}

	void Equipment::onLoad() {
		// icon_ = items->equipmentIcon(slot);
	}

	UsageResult Equipment::use() {
		if (self.lock()->equip(*this, stats.slot)) {
			identify();
			return UsageResult::SUCCESS;
		} else {
			return UsageResult::FAILURE;
		}
	}

}
