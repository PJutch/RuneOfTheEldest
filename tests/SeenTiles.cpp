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

#include "SeenTiles.hpp"

#include "Dungeon.hpp"
#include "Player.hpp"

#include <gtest/gtest.h>

#include <memory>

TEST(SeenTiles, visibilityEmpty) {
    auto dungeon = std::make_shared<Dungeon>();
    dungeon->resize(1);
    (*dungeon)[0].generateBlank({ 3, 3 });

    for (int x = 0; x < 3; ++x)
        for (int y = 0; y < 3; ++y)
            dungeon->at(x, y, 0) = Tile::EMPTY;

    auto player = std::make_shared<Player>();
    player->position({ 0, 2, 0 });

    SeenTiles seenTiles{ std::move(player), std::move(dungeon) };
    seenTiles.onGenerate();
    seenTiles.update();

    for (int x = 0; x < 3; ++x)
        for (int y = 0; y < 3; ++y)
            EXPECT_EQ(seenTiles.tileState({ x, y, 0 }), SeenTiles::TileState::VISIBLE);
}

TEST(SeenTiles, visibilityWall) {
    auto dungeon = std::make_shared<Dungeon>();
    dungeon->resize(1);
    (*dungeon)[0].generateBlank({ 3, 3 });

    for (int x = 0; x < 3; ++x)
         dungeon->at(x, 0, 0) = Tile::EMPTY;
    for (int x = 0; x < 3; ++x)
        dungeon->at(x, 1, 0) = Tile::WALL;
    for (int x = 0; x < 3; ++x)
        dungeon->at(x, 2, 0) = Tile::EMPTY;

    auto player = std::make_shared<Player>();
    player->position({ 1, 0, 0 });

    SeenTiles seenTiles{ std::move(player), std::move(dungeon) };
    seenTiles.onGenerate();
    seenTiles.update();

    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(seenTiles.tileState({ x, 0, 0 }), SeenTiles::TileState::VISIBLE);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(seenTiles.tileState({ x, 1, 0 }), SeenTiles::TileState::VISIBLE);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(seenTiles.tileState({ x, 2, 0 }), SeenTiles::TileState::UNSEEN);
}

TEST(SeenTiles, memorization) {
    auto dungeon = std::make_shared<Dungeon>();
    dungeon->resize(1);
    (*dungeon)[0].generateBlank({ 3, 3 });

    for (int x = 0; x < 3; ++x)
        dungeon->at(x, 0, 0) = Tile::EMPTY;
    for (int x = 0; x < 3; ++x)
        dungeon->at(x, 1, 0) = Tile::WALL;
    for (int x = 0; x < 3; ++x)
        dungeon->at(x, 2, 0) = Tile::EMPTY;

    auto player = std::make_shared<Player>();

    SeenTiles seenTiles{ player, std::move(dungeon) };
    seenTiles.onGenerate();

    player->position({ 1, 0, 0 });
    seenTiles.update();
    player->position({ 1, 2, 0 });
    seenTiles.update();

    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(seenTiles.tileState({ x, 0, 0 }), SeenTiles::TileState::MEMORIZED);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(seenTiles.tileState({ x, 1, 0 }), SeenTiles::TileState::VISIBLE);
    for (int x = 0; x < 3; ++x)
        EXPECT_EQ(seenTiles.tileState({ x, 2, 0 }), SeenTiles::TileState::VISIBLE);
}
