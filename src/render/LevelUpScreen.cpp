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
        const sf::Vector2f skillSize{256, 256};
    }

    void drawLevelupScreen(sf::RenderTarget& target, const render::AssetManager& assets,
                           const core::World& world, const core::XpManager& xpManager) {
        target.setView(createFullscreenView(1000.f, target.getSize()));

        auto skills = xpManager.availableSkills();

        sf::Vector2f screenSize = target.getView().getSize();

        float leftBoundary = (screenSize.x - skillSize.x * skills.size()) / 2;
        float skillXCenter = leftBoundary + skillSize.x / 2;
        for (const core::Skill* skill : skills) {
            drawRect(target, {skillXCenter - skillSize.x / 2, 300, skillSize.x, skillSize.y},
                sf::Color{32, 32, 32}, sf::Color{128, 128, 128}, 4.f);

            const sf::Texture& icon = skill->icon();
            sf::Vector2f iconCenter = util::geometry_cast<float>(icon.getSize()) / 2.f;
            drawSprite(target, {skillXCenter, 0.375f * screenSize.y}, iconCenter, icon, 1.0, 8.0);

            drawText(target, {skillXCenter, 0.5f * screenSize.y}, skill->name(), assets.font(), sf::Color::White, 30);

            skillXCenter += skillSize.x;
        }
    }

    void handleLevelupScreenEvent(sf::RenderTarget& target, core::XpManager& xpManager, sf::Event event) {
        if (event.type != event.MouseButtonPressed)
            return;

        target.setView(hudView(target.getSize()));
        sf::Vector2f clickPos = target.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

        auto skills = xpManager.availableSkills();

        sf::Vector2f screenSize = target.getView().getSize();

        float leftBoundary = (screenSize.x - skillSize.x * skills.size()) / 2;
        float skillXCenter = leftBoundary + skillSize.x / 2;
        for (const core::Skill* skill : skills) {
            sf::FloatRect currentSkillRect{skillXCenter - skillSize.x / 2, 300, skillSize.x, skillSize.y};

            if (currentSkillRect.contains(clickPos)) {
                xpManager.levelUp(skill);
                return;
            }

            skillXCenter += skillSize.x;
        }
    }
}
