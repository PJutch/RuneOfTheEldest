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

#ifndef BONUS_SPELL_HPP_
#define BONUS_SPELL_HPP_

#include "Spell.hpp"

#include "core/Effect/ConditionalBonus.hpp"

#include <string>
#include <memory>

namespace core {
	namespace Spells {
		class Bonus : public Spell {
		public:
			struct Stats {
				const sf::Texture* icon = nullptr;
				std::string name;

				StatBoosts boosts;
				double mana;
			};

			Bonus(Stats stats, const auto& env) :
				Spell{*stats.icon, env.id, stats.name}, mana{stats.mana},
				bonus{std::make_shared<Effect>(stats.boosts, *stats.icon, std::format("{}__spellBonus", env.id), stats.name)} {}

			UsageResult cast(bool useMana_ = true) final {
				isOn = !isOn;
				useMana = useMana_;
				return UsageResult::SUCCESS;
			}

			[[nodiscard]] std::shared_ptr<Spell> clone() const final {
				auto cloned = std::make_shared<Bonus>(*this);
				cloned->bonus->spell(cloned);
				return cloned;
			}

			void owner(std::weak_ptr<Actor> newOwner) {
				newOwner.lock()->addEffect(bonus->clone());
				owner_ = std::move(newOwner);
			}

			sf::Color frameColor() const final {
				if (isOn) {
					if (owner_.lock()->mana() > mana) {
						return sf::Color::Green;
					} else {
						return sf::Color::Red;
					}
				} else {
					return Spell::frameColor();
				}
			}

			[[nodiscard]] bool hasScroll() const final {
				return false;
			}
		private:
			class Effect : public ConditionalBonus {
			public:
				class RequirementNotMet : public util::RuntimeError {
				public:
					using RuntimeError::RuntimeError;
				};

				Effect(StatBoosts newBoosts, const sf::Texture& newIcon, std::string_view newId, std::string_view newName) :
					boosts_{newBoosts}, icon_{&newIcon}, id_{newId}, name_{newName} {
					if (boosts_.hp != 0)
						throw RequirementNotMet{"BonusSpell can't change max hp"};
					if (boosts_.mana != 0)
						throw RequirementNotMet{"BonusSpell can't change max mana"};
				}

				bool shouldApply() const final {
					auto spell = spell_.lock();
					return spell->isOn && owner_.lock()->mana() > spell->mana;
				}

				void update(double time) final {
					auto spell = spell_.lock();
					if (!spell->isOn)
						return;

					owner_.lock()->useMana(time * spell->mana);
				}

				void owner(std::weak_ptr<Actor> newOwner) final {
					owner_ = std::move(newOwner);
				}

				void spell(std::weak_ptr<Bonus> newSpell) {
					spell_ = std::move(newSpell);
				}

				bool isVisible() const final {
					return false;
				}

				std::unique_ptr<core::Effect> clone() const {
					return std::make_unique<Effect>(*this);
				}

				void parseData(std::string_view) final {
					throw UnexpectedData{};
				}

				[[nodiscard]] std::optional<std::string> stringify() const final {
					return std::nullopt;
				}

				[[nodiscard]] const sf::Texture& icon() const final {
					return *icon_;
				}

				[[nodiscard]] const std::string& id() const final {
					return id_;
				}

				[[nodiscard]] const std::string& name() const final {
					return name_;
				}

				[[nodiscard]] bool isSkill() const final {
					return true;
				}
			protected:
				[[nodiscard]] const StatBoosts& boosts() const final {
					return boosts_;
				}
			private:
				StatBoosts boosts_;
				const sf::Texture* icon_;
				std::string id_;
				std::string name_;
				std::weak_ptr<Bonus> spell_;
				std::weak_ptr<core::Actor> owner_;
			};

			bool isOn = false;
			bool useMana = true;
			double mana;
			std::shared_ptr<Effect> bonus;
			std::weak_ptr<core::Actor> owner_;
		};

		BOOST_DESCRIBE_STRUCT(Bonus::Stats, (), (icon, name, boosts, mana))
	}
}

#endif