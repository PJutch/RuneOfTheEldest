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

        enum class TooltipMode {
            UP_LEFT,
            UP_RIGHT
        };

        void drawTooltip(sf::RenderTarget& target, const AssetManager& assets,
                         const auto& element, TooltipMode mode) {
            auto text = createText(element.name(), assets.font(), sf::Color::White, 30);

            auto tooltipSize = text.getLocalBounds().getSize() + sf::Vector2f{10.f, 10.f};
            auto tooltipPos = target.mapPixelToCoords(sf::Mouse::getPosition());

            switch (mode) {
            case TooltipMode::UP_LEFT: tooltipPos -= tooltipSize; break;
            case TooltipMode::UP_RIGHT: tooltipPos.y -= tooltipSize.y; break;
            default: TROTE_ASSERT(false, "unreachable");
            }

            drawRect(target, {tooltipPos, tooltipSize}, sf::Color{32, 32, 32}, sf::Color{128, 128, 128}, 3.f);

            text.setPosition(tooltipPos + sf::Vector2f{5.f, -5.f});
            target.draw(text);
        }

        void drawIcon(sf::RenderTarget& target, sf::FloatRect rect, sf::Color boundaryColor, const sf::Texture& icon) {
            drawRect(target, rect, sf::Color{32, 32, 32}, boundaryColor, 2.f);

            sf::Vector2f iconCenter = util::geometry_cast<float>(icon.getSize()) / 2.f;
            sf::Vector2f center = rect.getPosition() + rect.getSize() / 2.f;
            drawSprite(target, center, iconCenter, icon, 1.0, 2.f);
        }

        enum class IconMode {
            TOP_RIGHT,
            BOTTOM_LEFT,
            BOTTOM_RIGHT,
        };

        template <typename Elements, typename GetBounaryColor> 
            requires std::convertible_to<std::invoke_result_t<GetBounaryColor, std::ranges::range_value_t<Elements>, int>, sf::Color>
        void drawIcons(sf::RenderTarget& target, const AssetManager& assets, 
                       Elements&& elements, IconMode mode, GetBounaryColor&& getBoundaryColor) {
            target.setView(createFullscreenView(1000.f, target.getSize()));

            const sf::Vector2f screenSize = target.getView().getSize();
            const sf::Vector2f iconSize{32.f, 32.f};
            const float padding = 4.f;

            const float firstXCenter = (mode == IconMode::TOP_RIGHT || mode == IconMode::BOTTOM_RIGHT
                                       ? screenSize.x - padding - iconSize.x / 2
                                       :                padding + iconSize.x / 2);

            float x = firstXCenter;
            const float y = (mode == IconMode::TOP_RIGHT ? 50.f : 950.f);
            int i = 0;

            for (const auto& element : elements) {
                drawIcon(target, sf::FloatRect{sf::Vector2f{x, y} - iconSize / 2.f, iconSize}, getBoundaryColor(element, i), element.icon());

                x += (iconSize.x + 2 * padding) * (mode == IconMode::BOTTOM_LEFT ? 1 : -1);
                ++i;
            }

            x = firstXCenter;

            for (const auto& element : elements) {
                sf::FloatRect rect{sf::Vector2f{x, y} - iconSize / 2.f, iconSize};

                if (rect.contains(target.mapPixelToCoords(sf::Mouse::getPosition())))
                    drawTooltip(target, assets, element, mode == IconMode::BOTTOM_LEFT ? TooltipMode::UP_RIGHT : TooltipMode::UP_LEFT);

                x += (iconSize.x + 2 * padding) * (mode == IconMode::BOTTOM_LEFT ? 1 : -1);
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
        drawIcons(target, assets, world.player().effects() 
                                | std::views::transform([](const auto& ptr) -> const auto& { return *ptr; })
                                | std::views::filter([](const auto& effect) { return effect.isVisible() && effect.isSkill(); }),
                  IconMode::BOTTOM_RIGHT, [](const auto&, int) { return sf::Color{128, 128, 128}; });
        drawIcons(target, assets, world.player().effects()
                                | std::views::transform([](const auto& ptr) -> const auto& { return *ptr; })
                                | std::views::filter([](const auto& effect) { return effect.isVisible() && !effect.isSkill(); }),
                  IconMode::TOP_RIGHT, [](const auto&, int) { return sf::Color{128, 128, 128}; });
        drawIcons(target, assets, world.player().spells()
                                | std::views::transform([](const auto& ptr) -> const auto& { return *ptr; }),
                  IconMode::BOTTOM_LEFT, [&](const auto& spell, int i) { 
            if (auto currentSpell = world.player().controller().currentSpell())
                if (i == currentSpell)
                    return sf::Color{255, 255, 0};
            return spell.frameColor();
        });
    }
}
