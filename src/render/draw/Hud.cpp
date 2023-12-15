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
#include "render/AssetManager.hpp"

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

            const float skillY = 950.f;
            const float effectY = 50.f;

            float rightmostXCenter = screenSize.x - padding - iconSize.x / 2;
            float skillXCenter = rightmostXCenter;
            float effectXCenter = rightmostXCenter;

            for (const auto& skill : skills) {
                if (!skill->isVisible())
                    continue;

                float& x = skill->isSkill() ? skillXCenter : effectXCenter;
                float y = skill->isSkill() ? skillY : effectY;

                sf::FloatRect skillRect{sf::Vector2f{x, y} - iconSize / 2.f, iconSize};

                drawRect(target, skillRect, sf::Color{32, 32, 32}, sf::Color{128, 128, 128}, 2.f);

                const sf::Texture& icon = skill->icon();
                sf::Vector2f iconCenter = util::geometry_cast<float>(icon.getSize()) / 2.f;
                drawSprite(target, {x, y}, iconCenter, icon, 1.0, 2.f);

                x -= iconSize.x + 2 * padding;
            }

            skillXCenter = rightmostXCenter;
            effectXCenter = rightmostXCenter;

            for (const auto& skill : skills) {
                float& x = skill->isSkill() ? skillXCenter : effectXCenter;
                float y = skill->isSkill() ? skillY : effectY;

                sf::FloatRect skillRect{sf::Vector2f{x, y} - iconSize / 2.f, iconSize};

                if (skillRect.contains(target.mapPixelToCoords(sf::Mouse::getPosition())))
                    drawSkillTooltip(target, assets, skill);

                x -= iconSize.x + 2 * padding;
            }
        }

        void drawSpellTooltip(sf::RenderTarget& target, const AssetManager& assets,
                              std::shared_ptr<core::Spell> skill) {
            auto text = createText(skill->name(), assets.font(), sf::Color::White, 30);

            auto tooltipSize = text.getLocalBounds().getSize() + sf::Vector2f{10.f, 10.f};
            auto tooltipPos = target.mapPixelToCoords(sf::Mouse::getPosition()) - sf::Vector2f{0, tooltipSize.y};

            drawRect(target, {tooltipPos, tooltipSize}, sf::Color{32, 32, 32}, sf::Color{128, 128, 128}, 3.f);

            text.setPosition(tooltipPos + sf::Vector2f{5.f, -5.f});
            target.draw(text);
        }

        void drawSpellIcons(sf::RenderTarget& target, const AssetManager& assets, const core::World& world) {
            target.setView(createFullscreenView(1000.f, target.getSize()));

            const sf::Vector2f iconSize{32.f, 32.f};
            float padding = 4.f;

            float leftmostXCenter = padding + iconSize.x / 2;
            float x = leftmostXCenter;
            const float y = 950.f;

            for (int i = 0; i < std::ssize(world.player().spells()); ++ i) {
                const auto& spell = world.player().spells()[i];
                sf::FloatRect spellRect{sf::Vector2f{x, y} - iconSize / 2.f, iconSize};

                sf::Color boundaryColor = spell->frameColor();
                if (auto currentSpell = world.player().controller().currentSpell())
                    if (i == currentSpell)
                        boundaryColor = {255, 255, 0};

                drawRect(target, spellRect, sf::Color{32, 32, 32}, boundaryColor, 2.f);

                const sf::Texture& icon = spell->icon();
                sf::Vector2f iconCenter = util::geometry_cast<float>(icon.getSize()) / 2.f;
                drawSprite(target, {x, y}, iconCenter, icon, 1.0, 2.f);

                x += iconSize.x + 2 * padding;
            }

            x = leftmostXCenter;

            for (const auto& spell : world.player().spells()) {
                sf::FloatRect spellRect{sf::Vector2f{x, y} - iconSize / 2.f, iconSize};

                if (spellRect.contains(target.mapPixelToCoords(sf::Mouse::getPosition())))
                    drawSpellTooltip(target, assets, spell);

                x += iconSize.x + 2 * padding;
            }
        }
    }

    std::optional<int> clickedSpell(sf::Vector2i clickPos, sf::RenderTarget& target, const core::Actor& player) {
        target.setView(createFullscreenView(1000.f, target.getSize()));

        const sf::Vector2f iconSize{32.f, 32.f};
        float padding = 4.f;

        float leftmostXCenter = padding + iconSize.x / 2;
        float x = leftmostXCenter;
        const float y = 950.f;

        for (int i = 0; i < std::ssize(player.spells()); ++i) {
            sf::FloatRect spellRect{sf::Vector2f{x, y} - iconSize / 2.f, iconSize};

            if (spellRect.contains(target.mapPixelToCoords(clickPos)))
                return i;

            x += iconSize.x + 2 * padding;
        }
        return std::nullopt;
    }

    void drawHud(sf::RenderTarget& target, const AssetManager& assets, 
                 const core::World& world, const core::XpManager& xpManager) {
        drawXpBar(target, xpManager);
        drawSkillIcons(target, assets, world);
        drawSpellIcons(target, assets, world);
    }
}
