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

        ActorImpact(double damage_, DamageType damageType_, double accuracy_) :
            damage{damage_}, damageType{damageType_}, accuracy{accuracy_} {}

        ActorImpact(const Effect* effect_, double accuracy_) :
            effect{effect_}, accuracy{accuracy_} {}

        void apply(core::Actor& actor) const {
            if (actor.beAttacked(damage, accuracy, damageType) && effect) {
                actor.addEffect(effect->clone());
            }
        }
    private:
        double damage = 0.0;
        DamageType damageType = DamageType::PHYSICAL;

        const Effect* effect = nullptr;

        double accuracy = 0.0;
    };
}

#endif