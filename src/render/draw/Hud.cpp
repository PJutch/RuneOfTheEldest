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

#include "Hud.hpp"

#include "Primitives.hpp"
#include "render/View.hpp"

#include "core/XpManager.hpp"
#include "core/World.hpp"
#include "core/Actor.hpp"

namespace render {
    namespace {
        void drawXpBar(sf::RenderTarget& target, const core::XpManager& xpManager) {
            target.setView(sf::View{{0.f, 0.f, 1.f, 1.f}});

            auto xpPercent = static_cast<float>(xpManager.xpPercentUntilNextLvl());
            sf::Vector2f size{xpPercent, 1.f / 128.f};
            sf::FloatRect rect{0, 1.f - size.y, size.x, size.y};

            drawRect(target, rect, {255, 128, 0});
        }

        void drawSkillTooltip(sf::RenderTarget& target, const AssetManager& assets,
                              const std::unique_ptr<core::Effect>& skill) {
            auto text = createText(skill->name(), assets.font(), sf::Color::White, 30);

            auto tooltipSize = text.getLocalBounds().getSize() + sf::Vector2f{10.f, 10.f};
            auto tooltipPos = target.mapPixelToCoords(sf::Mouse::getPosition()) - tooltipSize;

            drawRect(target, {tooltipPos, tooltipSize}, sf::Color{32, 32, 32}, sf::Color{128, 128, 128}, 3.f);

            text.setPosition(tooltipPos + sf::Vector2f{5.f, -5.f});
            target.draw(text);
        }

        void drawSkillIcons(sf::RenderTarget& target, const AssetManager& assets, const core::World& world) {
            const auto& skills = world.player().effects();

            target.setView(createFullscreenView(1000.f, target.getSize()));

            sf::Vector2f screenSize = target.getView().getSize();
            const sf::Vector2f iconSize{32.f, 32.f};
            float padding = 4.f;
            float skillXCenter = screenSize.x - padding - iconSize.x / 2;
            for (const auto& skill : skills) {
                if (!skill->isSkill())
                    continue;

                const float iconY = 950.f;
                sf::FloatRect skillRect{sf::Vector2f{skillXCenter, iconY} - iconSize / 2.f, iconSize};

                drawRect(target, skillRect, sf::Color{32, 32, 32}, sf::Color{128, 128, 128}, 2.f);

                const sf::Texture& icon = skill->icon();
                sf::Vector2f iconCenter = util::geometry_cast<float>(icon.getSize()) / 2.f;
                drawSprite(target, {skillXCenter, iconY}, iconCenter, icon, 1.0, 2.f);

                if (skillRect.contains(target.mapPixelToCoords(sf::Mouse::getPosition())))
                    drawSkillTooltip(target, assets, skill);

                skillXCenter -= iconSize.x + 2 * padding;
            }
        }
    }

    void drawHud(sf::RenderTarget& target, const AssetManager& assets, 
                 const core::World& world, const core::XpManager& xpManager) {
        drawXpBar(target, xpManager);
        drawSkillIcons(target, assets, world);
    }
}
