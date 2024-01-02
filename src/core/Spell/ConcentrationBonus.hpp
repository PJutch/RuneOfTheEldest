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

#ifndef CONCENTRATION_BONUS_SPELL_HPP_
#define CONCENTRATION_BONUS_SPELL_HPP_

#include "Spell.hpp"

#include "core/fwd.hpp"
#include "core/DamageType.hpp"
#include "core/Position.hpp"
#include "core/World.hpp"
#include "core/Actor.hpp"

#include "render/ParticleManager.hpp"
#include "render/coords.hpp"

#include "util/raycast.hpp"
#include "util/random.hpp"

namespace sf {
	class Texture;
}

#include <boost/describe.hpp>

#include <string>
#include <memory>

namespace core {
	class ConcentrationBonusSpell : 
		public Spell, public std::enable_shared_from_this<ConcentrationBonusSpell> {
	public:
		ConcentrationBonusSpell(ConditionalBonus::Bonuses bonuses, double mana_, const sf::Texture& particleTexture_, 
				const sf::Texture& icon, std::string_view id, std::string_view name,
				std::shared_ptr<render::ParticleManager> particles_) :
			Spell{icon, id, name}, bonus{std::make_shared<Bonus>(bonuses, icon, std::format("{}__spellBonus", id), name)},
			mana{mana_}, particleTexture{&particleTexture_}, particles{std::move(particles_)} {}

		UsageResult cast(std::shared_ptr<Actor> self_, bool useMana = true) final {
			if (self_->castedSpell().get() == this || useMana && !self_->useMana(mana)) {
				return UsageResult::FAILURE;
			}

			self = std::move(self_);
			particles->add(std::make_unique<Particle>(shared_from_this(), particles));
			return UsageResult::SUCCESS;
		}

		bool continueCast() final {
			return isCasted();
		}

		void restartCast() final {
			particles->add(std::make_unique<Particle>(shared_from_this(), particles));
		}

		[[nodiscard]] std::shared_ptr<Spell> clone() const final {
			auto cloned = std::make_shared<ConcentrationBonusSpell>(*this);
			cloned->bonus->spell(cloned);
			return cloned;
		}

		void owner(std::weak_ptr<Actor> newOwner) {
			self = std::move(newOwner);
			self.lock()->addEffect(bonus->clone());
		}

		sf::Color frameColor() const final {
			return isCasted() ? sf::Color::Green : sf::Color{128, 128, 128};
		}
	private:
		class Particle : public render::ParticleManager::CustomParticle {
		public:
			Particle(std::shared_ptr<ConcentrationBonusSpell> spell_, std::weak_ptr<render::ParticleManager> particleManager_) :
				spell{std::move(spell_)}, particleManager {std::move(particleManager_)} {}

			void update(sf::Time) final {}

			void draw(sf::RenderTarget& target, core::Position<float> cameraPos) const {
				if (spell->self.expired()) {
					return;
				}

				auto tilePos = spell->self.lock()->position();
				auto screenPos = render::toScreen(util::geometry_cast<float>(util::getXY(tilePos)) + sf::Vector2f{0.5f, 0.5f});

				auto textureSize = util::geometry_cast<float>(spell->particleTexture->getSize());
				sf::Vector2f origin{textureSize.x / 2.f, textureSize.y - render::tileSize.y / 2.f};
				auto center = textureSize / 2.f;

				particleManager.lock()->drawParticle(target, cameraPos, 
					{screenPos - origin + center, tilePos.z}, 0.f, spell->particleTexture);
			}

			bool shouldBeDeleted() const {
				return !spell->isCasted();
			}
		private:
			std::shared_ptr<ConcentrationBonusSpell> spell;
			std::weak_ptr<render::ParticleManager> particleManager;
		};

		class Bonus : public ConditionalBonus {
		public:
			class RequirementNotMet : public util::RuntimeError {
			public:
				using RuntimeError::RuntimeError;
			};

			Bonus(ConditionalBonus::Bonuses bonuses, const sf::Texture& icon, std::string_view id, std::string_view name) :
					ConditionalBonus{bonuses, icon, id, name, false} {
				if (bonuses.hpBonus != 0)
					throw RequirementNotMet{"ConcentrationBonusSpell can't change max hp"};
				if (bonuses.manaBonus != 0) 
					throw RequirementNotMet{"ConcentrationBonusSpell can't change max mana"};
			}

			bool shouldApply() const final {
				return !spell_.expired() && spell_.lock()->isCasted();
			}

			void spell(std::weak_ptr<ConcentrationBonusSpell> newSpell) {
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
			std::weak_ptr<ConcentrationBonusSpell> spell_;
		};

		std::shared_ptr<Bonus> bonus;

		double mana;
		const sf::Texture* particleTexture;

		std::weak_ptr<Actor> self;

		std::shared_ptr<render::ParticleManager> particles;

		bool isCasted() const {
			return !self.expired() && self.lock()->castedSpell().get() == this;
		}
	};
}

#endif