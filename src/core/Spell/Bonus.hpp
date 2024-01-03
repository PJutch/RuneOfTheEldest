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
	class BonusSpell : public Spell {
	public:
		BonusSpell(ConditionalBonus::Bonuses bonuses, double mana_, 
				const sf::Texture& icon, std::string_view id, std::string_view name) :
			Spell{icon, id, name}, mana{mana_}, 
			bonus{std::make_shared<Bonus>(bonuses, icon, std::format("{}__spellBonus", id), name)} {}

		UsageResult cast(bool useMana_ = true) final {
			isOn = !isOn;
			useMana = useMana_;
			return UsageResult::SUCCESS;
		}

		[[nodiscard]] std::shared_ptr<Spell> clone() const final {
			auto cloned = std::make_shared<BonusSpell>(*this);
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

		[[nodiscatd]] bool hasScroll() const final {
			return false;
		}
	private:
		class Bonus : public ConditionalBonus {
		public:
			class RequirementNotMet : public util::RuntimeError {
			public:
				using RuntimeError::RuntimeError;
			};

			Bonus(ConditionalBonus::Bonuses bonuses, const sf::Texture& icon, std::string_view id, std::string_view name) :
					ConditionalBonus{bonuses, icon, id, name, false} {
				if (bonuses.hpBonus != 0)
					throw RequirementNotMet{"BonusSpell can't change max hp"};
				if (bonuses.manaBonus != 0)
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

			void spell(std::weak_ptr<BonusSpell> newSpell) {
				spell_ = std::move(newSpell);
			}

			bool isVisible() const final {
				return false;
			}

			std::unique_ptr<Effect> clone() const {
				return std::make_unique<Bonus>(*this);
			}

			void parseData(std::string_view) final {
				throw UnexpectedData{};
			}

			[[nodiscard]] std::optional<std::string> stringify() const final {
				return std::nullopt;
			}
		private:
			std::weak_ptr<BonusSpell> spell_;
			std::weak_ptr<core::Actor> owner_;
		};

		bool isOn = false;
		bool useMana = true;
		double mana;
		std::shared_ptr<Bonus> bonus;
		std::weak_ptr<core::Actor> owner_;
	};
}

#endif