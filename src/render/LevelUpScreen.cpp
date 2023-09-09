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

#include "LevelUpScreen.hpp"

#include "Primitives.hpp"
#include "View.hpp"

#include "core/World.hpp"
#include "core/XpManager.hpp"

namespace render {
    namespace {
        const float skillWidth = 256;
        const float skillHeight = 256;
    }

    void drawLevelupScreen(sf::RenderTarget& target, const render::AssetManager& assets,
                           const core::World& world, const core::XpManager& xpManager) {
        target.setView(hudView(target.getSize()));

        auto skills = xpManager.availableSkills();

        sf::Vector2f screenSize = target.getView().getSize();

        float leftBoundary = (screenSize.x - skillWidth * skills.size()) / 2;
        float skillXCenter = leftBoundary + skillWidth / 2;
        for (const core::Skill* skill : skills) {
            drawRect(target, {skillXCenter - skillWidth / 2, 300, skillHeight, 250},
                sf::Color{32, 32, 32}, sf::Color{128, 128, 128}, 4.f);

            const sf::Texture& icon = skill->icon();
            drawSprite(target, {skillXCenter, 3 * screenSize.y / 8}, util::geometry_cast<float>(icon.getSize()) / 2.f,
                icon, 1.0, 8.0);

            drawText(target, {skillXCenter, screenSize.y / 2}, skill->name(), assets.font(), sf::Color::White, 30);

            skillXCenter += skillWidth;
        }
    }

    void handleLevelupScreenEvent(sf::RenderTarget& target, core::XpManager& xpManager, sf::Event event) {
        if (event.type != event.MouseButtonPressed)
            return;

        target.setView(hudView(target.getSize()));
        sf::Vector2f clickPos = target.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

        auto skills = xpManager.availableSkills();

        sf::Vector2f screenSize = target.getView().getSize();

        float leftBoundary = (screenSize.x - skillWidth * skills.size()) / 2;
        float skillXCenter = leftBoundary + skillWidth / 2;
        for (const core::Skill* skill : skills) {
            sf::FloatRect currentSkillRect{skillXCenter - skillWidth / 2, 300, skillWidth, skillHeight};

            if (currentSkillRect.contains(clickPos)) {
                xpManager.levelUp(skill);
                return;
            }

            skillXCenter += skillWidth;
        }
    }
}
