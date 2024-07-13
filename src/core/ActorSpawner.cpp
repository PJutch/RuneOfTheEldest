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

#include "ActorSpawner.hpp"

#include "Item.hpp"

#include "Controller/EnemyAi.hpp"
#include "Controller/PlayerController.hpp"

#include "render/AssetManager.hpp"

#include "util/parse.hpp"
#include "util/parseKeyValue.hpp"
#include "util/stringify.hpp"
#include "util/filesystem.hpp"
#include "util/Map.hpp"
#include "util/case.hpp"

#include <boost/describe.hpp>
#include <boost/mp11.hpp>

namespace core {
    namespace {
        class LoadError : public util::RuntimeError {
        public:
            LoadError(const std::string& message, util::Stacktrace currentStacktrace = {}) noexcept :
                RuntimeError{message, std::move(currentStacktrace)} {}
        };

        std::string unknownParamsMessage(std::unordered_map<std::string, std::string> params) {
            std::string message = "Unknown params: ";
            for (auto [name, value] : params)
                message += std::format("\"{}\" ", name);
            return message;
        }

        class UnknownParamsError : public LoadError {
        public:
            UnknownParamsError(std::unordered_map<std::string, std::string> params,
                util::Stacktrace currentStacktrace = {}) noexcept :
                LoadError{unknownParamsMessage(params), std::move(currentStacktrace)} {}
        };

        class EffectNotFound : public LoadError {
        public:
            EffectNotFound(std::string_view name, util::Stacktrace currentStacktrace = {}) noexcept :
                LoadError{std::format("Effect \"{}\" not found", name), std::move(currentStacktrace)} {}
        };

        class SpellNotFound : public LoadError {
        public:
            SpellNotFound(std::string_view name, util::Stacktrace currentStacktrace = {}) noexcept :
                LoadError{std::format("Spell \"{}\" not found", name), std::move(currentStacktrace)} {}
        };

        class ItemNotFound : public LoadError {
        public:
            ItemNotFound(std::string_view name, util::Stacktrace currentStacktrace = {}) noexcept :
                LoadError{std::format("Item \"{}\" not found", name), std::move(currentStacktrace)} {}
        };
    }
}

namespace JutchsON {
    template <>
    struct Parser<sf::Time> {
        ParseResult<sf::Time> operator() (StringView s, const auto& env, Context context) {
            return parse<float>(s, env, context).map([](float t) {
                return sf::seconds(t);
            });
        }
    };

    template <>
    struct Parser<core::DamageType> {
        ParseResult<core::DamageType> operator() (StringView s, const auto& env, Context context) {
            return parse<std::string>(s, env, context).then([&](std::string_view name) {
                auto result = ParseResult<core::DamageType>::makeError(s.location(),
                std::format("Unknown damage type {}", s.asStd()));
            boost::mp11::mp_for_each<boost::describe::describe_enumerators<core::DamageType>>([&](const auto& d) {
                if (d.name == name) {
                    result = d.value;
                }
            });
            return result;
            });
        }
    };

    template <>
    struct Parser<const core::Effect*> {
        ParseResult<const core::Effect*> operator() (StringView s, const auto& env, Context context) {
            return parse<std::string>(s, env, context).then([&](std::string_view name) -> ParseResult<const core::Effect*> {
                if (auto effect = env.effects->findEffect(name)) {
                    return effect;
                }
                return ParseResult<const core::Effect*>::makeError(s.location(),
                    std::format("Unknown effect {}", s.asStd()));
            });
        }
    };

    template <>
    struct Parser<std::shared_ptr<core::Spell>> {
        ParseResult<std::shared_ptr<core::Spell>> operator() (StringView s, const auto& env, Context context) {
            return parse<std::string>(s, env, context).then([&](std::string_view name) 
                    -> ParseResult<std::shared_ptr<core::Spell>> {
                if (auto spell = env.spells->findSpell(name)) {
                    return spell;
                }
                return ParseResult<std::shared_ptr<core::Spell>>::makeError(s.location(),
                    std::format("Unknown spell {}", s.asStd()));
            });
        }
    };
}

namespace core {
    namespace {
        struct Env {
            std::string_view id;
            std::shared_ptr<EffectManager> effects;
            std::shared_ptr<SpellManager> spells;
            std::shared_ptr<render::AssetManager> assets;
        };

        std::string errorString(const std::vector<JutchsON::ParseError>& errors) {
            std::string res;
            res.append(std::format("Parse failed with {} errors", std::ssize(errors)));
            for (const JutchsON::ParseError& error : errors) {
                res.append(std::format("\n{}:{} {}", error.location.line, error.location.column, error.what));
            }
            return res;
        }

        class ParseError : public util::RuntimeError {
        public:
            ParseError(const std::vector<JutchsON::ParseError>& errors, util::Stacktrace currentStacktrace = {}) noexcept :
                RuntimeError{errorString(errors), std::move(currentStacktrace)} {}
        };
    }

    ActorSpawner::ActorSpawner(std::shared_ptr<World> world_, std::shared_ptr<XpManager> xpManager_,
        std::shared_ptr<EffectManager> effectManager_, std::shared_ptr<SpellManager> spellManager_,
        std::shared_ptr<ItemManager> itemManager_,
        render::Context renderContext_, util::LoggerFactory& loggerFactory,
        util::RandomEngine& randomEngine_,
        std::shared_ptr<util::Raycaster> raycaster_) :
            world{std::move(world_)}, xpManager{std::move(xpManager_)},
            effectManager{std::move(effectManager_)}, spellManager{std::move(spellManager_)}, 
            itemManager{std::move(itemManager_)},
            renderContext{std::move(renderContext_)},
            raycaster{std::move(raycaster_)},
            randomEngine{&randomEngine_}, logger{loggerFactory.create("actors")} {
        logger->info("Loading...");

        std::filesystem::path basePath = "resources/descriptions/Actors/";
        util::forEachFile(basePath, [&, this](std::ifstream& file, const std::filesystem::path& path) {
            std::string id = util::toIdentifier(path, basePath);
            logger->info("Loading {} spec from {}...", id, path.generic_string());

            if (auto loaded = JutchsON::parse<ActorSpawner::ActorData>(JutchsON::readWholeFile(path),
                    Env{id, effectManager, spellManager, renderContext.assets})) {
                loaded->stats.id = id;
                actorData.try_emplace(id, *loaded);
            } else {
                throw ParseError{loaded.errors()};
            }
        });
        logger->info("Loaded");
    }

    namespace {
        class UnknownControllerError : public util::RuntimeError {
        public:
            UnknownControllerError(std::string_view type, util::Stacktrace currentStacktrace = {}) noexcept :
                util::RuntimeError{std::format("Unknow controller {}", type), std::move(currentStacktrace)} {}
        };
    }

    std::unique_ptr<Controller> ActorSpawner::createController(std::shared_ptr<Actor> actor, std::string_view s) const {
        auto [type, data] = util::parseKeyValuePair(s);

        if (type == "player") {
            return std::make_unique<PlayerController>(actor, raycaster, renderContext);
        } else if (type == "enemy") {
            if (data.empty()) {
                return std::make_unique<EnemyAi>(actor, raycaster);
            }

            util::KeyValueVisitor visitor;
            EnemyAi::State state;

            visitor.key("targetPosition").unique().required().callback([&](std::string_view data) {
                state.targetPosition = util::parseVector3i(data);
            });

            visitor.key("targetPriority").unique().required().callback([&](std::string_view data) {
                state.targetPriority = util::parseReal(data);
            });

            visitor.key("checkStairs").unique().required().callback([&](std::string_view data) {
                state.checkStairs = util::parseBool(data);
            });

            visitor.key("wandering").unique().required().callback([&](std::string_view data) {
                state.checkStairs = util::parseBool(data);
            });

            util::forEackInlineKeyValuePair(data, visitor);
            visitor.validate();

            return std::make_unique<EnemyAi>(state, actor, raycaster);
        } else {
            throw UnknownControllerError{type};
        }
    }

    void ActorSpawner::spawn() {
        logger->info("Spawning...");
        for (int level = 0; level < world->tiles().shape().z; ++level)
            for (const auto& [id, data] : actorData)
                if (data.minLevel <= level && (!data.maxLevel || level <= data.maxLevel)) {
                    int count = std::uniform_int_distribution{data.minOnLevel, data.maxOnLevel}(*randomEngine);
                    for (int i = 0; i < count; ++i)
                        if (auto position = world->randomPositionAt(level, 1000, &World::isFree)) {
                            auto enemy = std::make_shared<Actor>(data.stats, *position, world, xpManager,
                                renderContext.particles, randomEngine);
                            enemy->controller(createController(enemy, data.controller));

                            if (data.effectToAdd)
                                enemy->addEffect(data.effectToAdd->clone());
                            for (const auto& spell : data.spellsToAdd)
                                enemy->addSpell(spell->clone());

                            world->addActor(std::move(enemy));
                        }
                }
        logger->info("Spawned");
    }

    namespace {
        class UnknownActorType : public util::RuntimeError {
        public:
            UnknownActorType(std::string_view type, util::Stacktrace currentStacktrace = {}) noexcept :
                util::RuntimeError{std::format("Can't find description of actor type {}", type), std::move(currentStacktrace)} {}
        };
    }

    namespace {
        class UnknownEquipmentSlotError : public util::RuntimeError {
        public:
            UnknownEquipmentSlotError(std::string_view equipmentSlot, util::Stacktrace currentStacktrace = {}) noexcept :
                RuntimeError{std::format("Unknown equipment slot \"{}\"", equipmentSlot),
                std::move(currentStacktrace)} {}
        };

        EquipmentSlot getEquipmentSlot(std::string_view name) {
            if (auto v = util::getOptional(equipmentSlotByName, std::string{name}))
                return *v;
            else
                throw UnknownEquipmentSlotError{name};
        }
    }

    std::shared_ptr<core::Actor> ActorSpawner::parseActor(std::string_view s) {
        util::KeyValueVisitor visitor;
        auto result = std::make_shared<core::Actor>(world, xpManager, renderContext.particles, randomEngine);

        visitor.key("type").unique().required().callback([&](std::string_view type) {
            if (auto data = actorData.find(std::string{type}); data != actorData.end()) {
                result->stats(data->second.stats);
            } else {
                throw UnknownActorType{type};
            }
        });

        visitor.key("position").unique().required().callback([&](std::string_view data) {
            result->position(util::parseVector3i(data));
        });

        visitor.key("controller").unique().required().callback([&](std::string_view data) {
            result->controller(createController(result, data));
        });

        visitor.key("nextTurn").unique().required().callback([&](std::string_view data) {
            result->nextTurn(util::parseReal(data));
        });

        visitor.key("hp").unique().required().callback([&](std::string_view data) {
            result->hpUnscaled(util::parseReal(data));
        });

        visitor.key("mana").unique().required().callback([&](std::string_view data) {
            result->manaUnscaled(util::parseReal(data));
        });

        visitor.key("effect").callback([&](std::string_view s) {
            auto [id, data] = util::parseKeyValuePair(s);

            if (auto effect = effectManager->findEffect(id)) {
                auto newEffect = effect->clone();
                newEffect->parseData(data);
                result->addEffect(std::move(newEffect));
            } else {
                throw EffectNotFound{id};
            }
        });

        visitor.key("spell").callback([&](std::string_view s) {
            spellsToAdd.emplace_back(result, std::string{s});
        });

        visitor.key("item").callback([&](std::string_view s) {
            auto [id, data] = util::parseKeyValuePair(s);

            if (auto item = itemManager->newItem(id)) {
                item->parseData(data);
                result->addItem(std::move(item));
            } else {
                throw ItemNotFound{id};
            }
        });

        visitor.key("casted").callback([&](std::string_view s) {
            if (auto stripped = util::strip(s); !stripped.empty() && util::isDigit(stripped[0])) {
                spellsToCast.emplace_back(result, util::parseInt(stripped));
            } else {
                spellsToCast.emplace_back(result, std::string{stripped});
            }
        });

        visitor.key("equiped").callback([&](std::string_view s) {
            auto [slotTypeStr, rest1] = util::parseKeyValuePair(s);
            auto [slotIndexStr, rest2] = util::parseKeyValuePair(rest1);
            auto [id, data] = util::parseKeyValuePair(rest2);

            auto slotType = getEquipmentSlot(slotTypeStr);
            int slotIndex = util::parseInt(slotIndexStr);

            if (std::unique_ptr<Equipment> item{dynamic_cast<Equipment*>(itemManager->newItem(id).release())}) {
                item->parseData(data);
                result->setEquipment(std::move(item), slotType, slotIndex);       
            } else {
                throw ItemNotFound{id};
            }
        });

        util::forEackKeyValuePair(s, visitor);
        visitor.validate();

        return result;
    }

    std::string ActorSpawner::stringifyActor(const core::Actor& actor) const {
        std::string result = std::format(
            "type {}\nposition {}\nnextTurn {}\nhp {}\nmana {}\ncontroller {}\n",
            actor.stats().id,
            util::stringifyVector3(actor.position()),
            actor.nextTurn(), actor.hpUnscaled(), actor.manaUnscaled(),
            actor.controller().stringify()
        );

        for (const auto& effect : actor.effects()) {
            if (auto data = effect->stringify()) {
                result.append(std::format("effect {}\n", *data));
            }
        }

        for (const auto& spell : actor.spells()) {
            result.append(std::format("spell {}\n", spell->stringify()));
        }

        if (auto iter = std::ranges::find(actor.spells(), actor.castedSpell()); iter != actor.spells().end()) {
            result.append(std::format("casted {}\n", iter - actor.spells().begin()));
        } else if (actor.castedSpell()) {
            result.append(std::format("casted {}\n", actor.castedSpell()->stringify()));
        }

        for (const auto& item : actor.items()) {
            result.append(std::format("item {} {}\n", item->id(), item->stringifyData()));
        }

        boost::mp11::mp_for_each<boost::describe::describe_enumerators<EquipmentSlot>>([&](auto D) {
            const auto& slots = actor.equipment()[static_cast<int>(D.value)];
            for (int i = 0; i < std::ssize(slots); ++i) {
                if (slots[i]) {
                    result.append(std::format("equiped {} {} {} {}\n", util::toLower(D.name), i, slots[i]->id(), slots[i]->stringifyData()));
                }
            }
        });

        return result;
    }

    void ActorSpawner::onSaveLoaded() {
        for (const auto& [actor, spellData] : spellsToAdd) {
            auto [id, data] = util::parseKeyValuePair(spellData);

            if (auto spell = spellManager->findSpell(id)) {
                auto newSpell = spell->clone();
                newSpell->parseData(data);
                actor->addSpell(std::move(newSpell));
            } else {
                throw SpellNotFound{id};
            }
        }

        for (const auto& [actor, spellData] : spellsToCast) {
            if (auto i = std::get_if<int>(&spellData)) {
                actor->castedSpell(actor->spells()[*i]);
                actor->spells()[*i]->restartCast();
            } else if (auto spellStr = std::get_if<std::string>(&spellData)) {
                auto [id, data] = util::parseKeyValuePair(*spellStr);

                if (auto spell = spellManager->findSpell(id)) {
                    auto newSpell = spell->clone();
                    newSpell->parseData(data);
                    actor->castedSpell(newSpell);
                    newSpell->owner(actor);
                    newSpell->restartCast();
                } else {
                    throw SpellNotFound{id};
                }
            } else {
                TROTE_ASSERT(false, "unreachable");
            }
        }
    }
}
