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

#ifndef SPELL_ACTOR_IMPACT_HPP_
#define SPELL_ACTOR_IMPACT_HPP_

#include "core/DamageType.hpp"
#include "core/Actor.hpp"

namespace core {
    class ActorImpact {
    public:
        ActorImpact() = default;

        ActorImpact(double newDamage, DamageType newDamageType, double newAccuracy) :
            damage_{newDamage}, damageType_{newDamageType}, accuracy_{newAccuracy} {}

        ActorImpact(const Effect* newEffect, double newAccuracy) :
            effect_{newEffect}, accuracy_{newAccuracy} {}

        void apply(core::Actor& actor) const {
            if (actor.beAttacked(damage_, accuracy_, damageType_) && effect_) {
                actor.addEffect(effect_->clone());
            }
        }

        void accuracy(double newAccuracy) {
            accuracy_ = newAccuracy;
        }

        void damage(double newDamage, DamageType newDamageType) {
            damage_ = newDamage;
            damageType_ = newDamageType;
        }

        void effect(const Effect* newEffect) {
            effect_ = newEffect;
        }
    private:
        double damage_ = 0.0;
        DamageType damageType_ = DamageType::PHYSICAL;

        const Effect* effect_ = nullptr;

        double accuracy_ = 0.0;
    };
}

#endif