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

#include "render/View.hpp"

#include <gtest/gtest.h>

TEST(View, createFullscreenViewHeight) {
    float height = 512.f;
    EXPECT_EQ(render::createFullscreenView(height, sf::Vector2f{2048.f, 1024.f}).getSize().y, height);
}

TEST(View, createFullscreenViewAspectRatio) {
    sf::Vector2f screenSize{ 2048.f, 1024.f };
    sf::Vector2f viewSize = render::createFullscreenView(512.f, screenSize).getSize();
    EXPECT_EQ(viewSize.x / viewSize.y, screenSize.x / screenSize.y);
}

TEST(View, createFullscreenViewCenter) {
    sf::Vector2f center{ 20.f, 20.f };
    EXPECT_EQ(render::createFullscreenView(center, 512, sf::Vector2f{ 2048.f, 1024.f }).getCenter(), center);
}

TEST(View, createFullscreenViewCenterHeight) {
    float height = 512.f;
    EXPECT_EQ(render::createFullscreenView({ 20.f, 20.f }, height, sf::Vector2f{ 2048.f, 1024.f }).getSize().y, height);
}

TEST(View, createFullscreenViewCenterAspectRatio) {
    sf::Vector2f screenSize{ 2048.f, 1024.f };
    sf::Vector2f viewSize = render::createFullscreenView({ 20.f, 20.f }, 512.f, screenSize).getSize();
    EXPECT_EQ(viewSize.x / viewSize.y, screenSize.x / screenSize.y);
}
