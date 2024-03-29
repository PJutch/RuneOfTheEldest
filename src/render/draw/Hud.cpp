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
            UP_RIGHT,
            DOWN_LEFT,
            DOWN_RIGHT
        };

        void drawTooltip(sf::RenderTarget& target, const AssetManager& assets,
                         const auto& element, TooltipMode mode) {
            auto text = createText(element.name(), assets.font(), sf::Color::White, 30);

            auto tooltipSize = text.getLocalBounds().getSize() + sf::Vector2f{10.f, 10.f};
            auto tooltipPos = target.mapPixelToCoords(sf::Mouse::getPosition());

            switch (mode) {
            case TooltipMode::UP_LEFT: tooltipPos -= tooltipSize; break;
            case TooltipMode::UP_RIGHT: tooltipPos.y -= tooltipSize.y; break;
            case TooltipMode::DOWN_LEFT: tooltipPos.x -= tooltipSize.x; break;
            case TooltipMode::DOWN_RIGHT: break;
            default: TROTE_ASSERT(false, "unreachable");
            }

            drawRect(target, {tooltipPos, tooltipSize}, sf::Color{32, 32, 32}, sf::Color{128, 128, 128}, 3.f);

            text.setPosition(tooltipPos + sf::Vector2f{5.f, -5.f});
            target.draw(text);
        }

        const sf::Vector2f iconSize{32.f, 32.f};

        void drawIcon(sf::RenderTarget& target, sf::FloatRect rect, sf::Color boundaryColor, const sf::Texture* icon) {
            drawRect(target, rect, sf::Color{32, 32, 32}, boundaryColor, 2.f);

            if (icon) {
                sf::Vector2f iconCenter = util::geometry_cast<float>(icon->getSize()) / 2.f;
                sf::Vector2f center = rect.getPosition() + rect.getSize() / 2.f;
                drawSprite(target, center, iconCenter, *icon, 1.0, iconSize.x / icon->getSize().x);
            }
        }

        enum class IconMode {
            TOP_RIGHT,
            TOP_LEFT,
            TOP_LEFT_ROW2,
            BOTTOM_LEFT,
            BOTTOM_RIGHT,
        };

        template <typename Elements, typename GetBounaryColor> 
            requires std::convertible_to<std::invoke_result_t<GetBounaryColor, std::ranges::range_value_t<Elements>, int>, sf::Color>
        void drawIcons(sf::RenderTarget& target, const AssetManager& assets, 
                       Elements&& elements, IconMode mode, GetBounaryColor&& getBoundaryColor) {
            target.setView(createFullscreenView(1000.f, target.getSize()));

            const sf::Vector2f screenSize = target.getView().getSize();
            const float padding = 4.f;

            const bool isLeft = mode == IconMode::BOTTOM_LEFT || mode == IconMode::TOP_LEFT || mode == IconMode::TOP_LEFT_ROW2;
            const float firstXCenter = (isLeft ? padding + iconSize.x / 2 : screenSize.x - padding - iconSize.x / 2);

            float x = firstXCenter;
            const float y = (mode == IconMode::TOP_RIGHT || mode == IconMode::TOP_LEFT ? 20.f 
                          : (mode == IconMode::TOP_LEFT_ROW2 ? 20.f + 2 * padding + iconSize.y : 970.f));
            int i = 0;

            for (const auto& element : elements) {
                const sf::Texture* icon = element ? &element->icon() : nullptr;
                drawIcon(target, sf::FloatRect{sf::Vector2f{x, y} - iconSize / 2.f, iconSize}, getBoundaryColor(element, i), icon);

                x += (iconSize.x + 2 * padding) * (isLeft ? 1 : -1);
                ++i;
            }

            x = firstXCenter;

            for (const auto& element : elements) {
                sf::FloatRect rect{sf::Vector2f{x, y} - iconSize / 2.f, iconSize};

                util::UnorderedMap<IconMode, TooltipMode> tooltipModes{{IconMode::BOTTOM_LEFT  , TooltipMode::UP_RIGHT  },
                                                                       {IconMode::BOTTOM_RIGHT , TooltipMode::UP_LEFT   },
                                                                       {IconMode::TOP_LEFT     , TooltipMode::DOWN_RIGHT},
                                                                       {IconMode::TOP_RIGHT    , TooltipMode::DOWN_LEFT },
                                                                       {IconMode::TOP_LEFT_ROW2, TooltipMode::DOWN_RIGHT}};

                if (element && rect.contains(target.mapPixelToCoords(sf::Mouse::getPosition()))) {
                    drawTooltip(target, assets, *element, tooltipModes[mode]);
                }

                x += (iconSize.x + 2 * padding) * (isLeft ? 1 : -1);
            }
        }
    }

    std::optional<int> clickedIcon(sf::Vector2i clickPos, sf::RenderTarget& target, int n_icons, IconMode mode) {
        target.setView(createFullscreenView(1000.f, target.getSize()));

        const sf::Vector2f screenSize = target.getView().getSize();
        float padding = 4.f;

        const bool isLeft = mode == IconMode::BOTTOM_LEFT || mode == IconMode::TOP_LEFT || mode == IconMode::TOP_LEFT_ROW2;
        const float firstXCenter = (isLeft ? padding + iconSize.x / 2 : screenSize.x - padding - iconSize.x / 2);

        float x = firstXCenter;
        const float y = (mode == IconMode::TOP_RIGHT || mode == IconMode::TOP_LEFT ? 20.f
            : (mode == IconMode::TOP_LEFT_ROW2 ? 20.f + 2 * padding + iconSize.y : 970.f));

        for (int i = 0; i < n_icons; ++i) {
            if (sf::FloatRect{sf::Vector2f{x, y} - iconSize / 2.f, iconSize}.contains(target.mapPixelToCoords(clickPos)))
                return i;

            x += (iconSize.x + 2 * padding) * (isLeft ? 1 : -1);
        }

        return std::nullopt;
    }

    void drawHud(sf::RenderTarget& target, const AssetManager& assets, 
                 const core::World& world, const core::XpManager& xpManager) {
        drawXpBar(target, xpManager);
        drawIcons(target, assets, world.player().effects() 
                                | std::views::filter([](const auto& effect) { return effect->isVisible() && effect->isSkill(); }),
                  IconMode::BOTTOM_RIGHT, [](const auto&, int) { return sf::Color{128, 128, 128}; });
        drawIcons(target, assets, world.player().effects()
                                | std::views::filter([](const auto& effect) { return effect->isVisible() && !effect->isSkill(); }),
                  IconMode::TOP_RIGHT, [](const auto&, int) { return sf::Color{128, 128, 128}; });
        drawIcons(target, assets, world.player().spells(), IconMode::BOTTOM_LEFT, [&](const auto& spell, int i) { 
            return std::visit([&]<typename T>(T v) {
                if constexpr (std::same_as<T, core::Controller::SelectedSpell>) {
                    if (v.i == i) {
                        return sf::Color::Yellow;
                    } else {
                        return spell->frameColor();
                    }
                } else {
                    return spell->frameColor();
                }
            }, world.player().controller().selectedAbility());
        });
        drawIcons(target, assets, world.player().equipment() | std::views::join,
            IconMode::TOP_LEFT_ROW2, [&](const auto&, int i) {
            return sf::Color{128, 128, 128};
        });
        drawIcons(target, assets, world.player().items(),
                  IconMode::TOP_LEFT, [&](const auto&, int i) { 
            return std::visit([&]<typename T>(T v) {
                if constexpr (std::same_as<T, core::Controller::SelectedItem>) {
                    if (v.i == i) {
                        return sf::Color::Yellow;
                    } else {
                        return sf::Color{128, 128, 128};
                    }
                } else {
                    return sf::Color{128, 128, 128};
                }
            }, world.player().controller().selectedAbility());
        });
    }

    std::optional<int> clickedSpell(sf::Vector2i clickPos, sf::RenderTarget& target, const core::Actor& player) {
        return clickedIcon(clickPos, target, std::ssize(player.spells()), IconMode::BOTTOM_LEFT);
    }

    std::optional<int> clickedItem(sf::Vector2i clickPos, sf::RenderTarget& target, const core::Actor& player) {
        return clickedIcon(clickPos, target, std::ssize(player.items()), IconMode::TOP_LEFT);
    }

    std::optional<std::pair<EquipmentSlot, int>> clickedEquipment(sf::Vector2i clickPos, 
                                                                  sf::RenderTarget& target, const core::Actor& player) {
        int nIcons = util::reduce(player.equipment() 
            | std::views::transform([](const auto& v) { return static_cast<int>(std::ssize(v)); }), 0, std::plus<>{});

        auto optIconIndex = clickedIcon(clickPos, target, nIcons, IconMode::TOP_LEFT_ROW2);
        if (!optIconIndex) {
            return std::nullopt;
        }

        int iconIndex = *optIconIndex;
        for (int i = 0; i < std::ssize(player.equipment()); ++i) {
            if (iconIndex < std::ssize(player.equipment()[i])) {
                return std::pair{static_cast<EquipmentSlot>(i), iconIndex};
            }
            
            iconIndex -= std::ssize(player.equipment()[i]);
        }
        return std::nullopt;
    }
}
