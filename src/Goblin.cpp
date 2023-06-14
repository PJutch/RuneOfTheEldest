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

#include "Goblin.hpp"

#include "pathfinding.hpp"

bool Goblin::act() {
	if (canSeePlayer())
		active = true;

	wantsSwap_ = true;
	if (active) {
		sf::Vector3i nextStep_ = nextStep(world().dungeon(), position(), player->position());
		if (nextStep_.z == 0)
			tryMoveInDirection(getXY(nextStep_), false);
		else
			tryMove(nextStep_, false);
	}
	wait(1);
	return true;
}

AiState Goblin::aiState() const noexcept {
	if (canSeePlayer())
		return AiState::ATTACKING;

	if (active) {
		return AiState::SEEKING;
	}
	else
		return AiState::INACTIVE;
}

void Goblin::spawnSingle(int level, std::shared_ptr<World> world, std::shared_ptr<Player> player_, RandomEngine& randomEngine) {
	sf::Vector2i position = world->dungeon()[level].randomPosition(randomEngine, [world, level](sf::Vector2i pos, const Level&) {
		return world->isFree(make3D(pos, level));
	});

	world->addActor(std::make_shared<Goblin>(make3D(position, level), world, std::move(player_), randomEngine));
}

void Goblin::spawnAll(std::shared_ptr<World> world, std::shared_ptr<Player> player_, RandomEngine& randomEngine) {
	for (int level = 0; level < world->dungeon().size(); ++level)
		for (int i = 0; i < std::uniform_int_distribution{ 5, 20 }(randomEngine); ++i)
			spawnSingle(level, world, player_, randomEngine);
}
