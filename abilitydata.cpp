#include "abilitydata.h"
#include "effectaction.h"

AbilityDatabase& AbilityDatabase::instance() {
    static AbilityDatabase db;
    return db;
}

AbilityDatabase::AbilityDatabase() { registerDefaults(); }

void AbilityDatabase::registerDefaults() {
    // ── 随机字母写入（每回合开始）──
    {
        auto a = std::make_unique<AbilityData>();
        a->id   = "random_letter_start";
        a->name = "随机触碰";
        a->desc = "每回合开始时，向字符串空间写入一个随机字母";
        a->trigger = Trigger::OnTurnStart;
        a->action = std::make_unique<WriteRandomLetterAction>();
        a->defaultDuration = 0; // 永久
        m_abilities[a->id] = std::move(a);
    }

    // ── 写入时获得格挡 ──
    {
        auto a = std::make_unique<AbilityData>();
        a->id   = "block_on_write";
        a->name = "护符共鸣";
        a->desc = "每次向字符串空间写入内容时，获得3点格挡";
        a->trigger = Trigger::OnWriteString;
        a->action = std::make_unique<BlockAction>(EffectValue::fixed(3));
        a->defaultDuration = 0; // 永久
        m_abilities[a->id] = std::move(a);
    }
    // ── 每打出一张牌，造成5点伤害（临时模板，不需要单独注册？实际由 AddAbilityAction 生成临时实例时使用同一 ID）──
    {
        auto a = std::make_unique<AbilityData>();
        a->id   = "temp_damage_per_card";
        a->name = "裂片爆发";
        a->desc = "本回合每打出一张牌，对敌人造成5点伤害";
        a->trigger = Trigger::OnCardPlayed;
        a->action = std::make_unique<DamageAction>(EffectValue::fixed(5));
        a->defaultDuration = 1; // 1回合
        m_abilities[a->id] = std::move(a);
    }

    // ── AdMath 力量 ──
    {
        auto a = std::make_unique<AbilityData>();
        a->id   = "admath_strength";
        a->name = "严谨性";
        a->desc = "每回合开始时获得1点力量";
        a->trigger = Trigger::OnTurnStart;
        a->action = std::make_unique<GainStrengthAction>(1);
        a->defaultDuration = 0;
        m_abilities[a->id] = std::move(a);
    }
    // ── 龙鳞再生（龙被动）──
    {
        auto a = std::make_unique<AbilityData>();
        a->id   = "dragon_scales";
        a->name = "龙鳞再生";
        a->desc = "每回合开始时获得2点格挡";
        a->trigger = Trigger::OnTurnStart;
        a->action = std::make_unique<BlockAction>(EffectValue::fixed(2));
        a->defaultDuration = 0; // 永久
        m_abilities[a->id] = std::move(a);
    }
    // ── Dragon 回血 ──
    {
        auto a = std::make_unique<AbilityData>();
        a->id   = "dragon_regen";
        a->name = "龙就是龙";
        a->desc = "回合结束时，若生命值未满，回复2点生命";
        a->trigger = Trigger::OnTurnEnd;
        a->action = std::make_unique<HealAction>(2);
        a->defaultDuration = 0;
        a->condition = std::make_unique<SelfHpNotFullCondition>();
        m_abilities[a->id] = std::move(a);
    }
    // ── Genius 破绽 ──
    {
        auto a = std::make_unique<AbilityData>();
        a->id   = "genius_break";
        a->name = "破绽";
        a->desc = "回合开始时，若玩家没有格挡，造成5点伤害";
        a->trigger = Trigger::OnTurnStart;
        a->action = std::make_unique<DamageAction>(EffectValue::fixed(5));
        a->defaultDuration = 0;
        a->condition = std::make_unique<PlayerBlockZeroCondition>();
        m_abilities[a->id] = std::move(a);
    }
}

const AbilityData* AbilityDatabase::abilityById(const QString& id) const {
    auto it = m_abilities.find(id);
    return (it != m_abilities.end()) ? it->second.get() : nullptr;
}