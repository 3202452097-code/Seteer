#include "CardData.h"
#include "EffectAction.h"
#include <QDebug>
CardDatabase& CardDatabase::instance() {
    static CardDatabase db;
    return db;
}
CardDatabase::CardDatabase() {
    registerDefaults();
}
void CardDatabase::registerDefaults() {
    // ── 打击（修改）──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "strike";
        c->name      = "打击";
        c->cost      = 1;
        c->desc      = "造成 6 点伤害。若字符串空间末尾为数字，消耗之并造成等量伤害。";
        c->imagePath = "";
        // 效果1：无条件 6 伤害
        c->effects.push_back(
            Effect(std::make_unique<DamageAction>(EffectValue::fixed(6)))
            );
        // 效果2：如果最后字符是数字 → 消耗并造成对应伤害
        c->effects.push_back(
            Effect(std::make_unique<LastCharIsDigitCondition>(),
                   std::make_unique<ConsumeLastDigitDamageAction>())
            );
        m_cards[c->id] = std::move(c);
    }
    // ── 防御（修改）──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "defend";
        c->name      = "防御";
        c->cost      = 1;
        c->desc      = "获得 5 点格挡。若字符串空间末尾为字母，消耗之并再获得 5 点格挡。";
        c->imagePath = "";
        // 效果1：无条件 5 格挡
        c->effects.push_back(
            Effect(std::make_unique<BlockAction>(EffectValue::fixed(5)))
            );
        // 效果2：如果最后字符是字母 → 消耗并再获得 5 格挡
        c->effects.push_back(
            Effect(std::make_unique<LastCharIsAlphaCondition>(),
                   std::make_unique<ConsumeLastAlphaBlockAction>(5))
            );
        c->isAttack = false;
        m_cards[c->id] = std::move(c);
    }
    // ── ★ 新增：随机字母 ──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "random_letter";
        c->name      = "随机字母";
        c->cost      = 0;
        c->desc      = "向字符串空间写入一个随机字母（A-Z）。";
        c->imagePath = "";   // 暂无卡图
        c->effects.push_back(
            Effect(std::make_unique<WriteRandomLetterAction>())
            );
        c->isAttack = false;
        m_cards[c->id] = std::move(c);
    }
    // ── ★ 新增：随机数字 ──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "random_digit";
        c->name      = "随机数字";
        c->cost      = 0;
        c->desc      = "向字符串空间写入一个随机数字（0-9）。";
        c->imagePath = "";   // 暂无卡图
        c->effects.push_back(
            Effect(std::make_unique<WriteRandomDigitAction>())
            );
        c->isAttack = false;
        m_cards[c->id] = std::move(c);
    }
    // ── ★ 清空爆破 ──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "clear_burst";
        c->name      = "清空爆破";
        c->cost      = 2;                // 你可按需调
        c->desc      = "清空字符串空间，造成原长度×4伤害。若原长度≥5，伤害翻倍。";
        c->imagePath = "";
        c->effects.push_back(
            Effect(std::make_unique<ClearStringDamageAction>())
            );
        m_cards[c->id] = std::move(c);
    }
    // ── ★ 字母连写 ──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "triple_letter";
        c->name      = "字母连写";
        c->cost      = 1;
        c->desc      = "向字符串空间写入三个随机字母。";
        c->imagePath = "";
        c->effects.push_back(Effect(std::make_unique<WriteRandomLetterAction>()));
        c->effects.push_back(Effect(std::make_unique<WriteRandomLetterAction>()));
        c->effects.push_back(Effect(std::make_unique<WriteRandomLetterAction>()));
        c->isAttack = false;
        m_cards[c->id] = std::move(c);
    }
    // ── ★ 数字差力量 ──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "digit_diff_strength";
        c->name      = "数字差力量";
        c->cost      = 1;
        c->desc      = "若字符串最后两位均为数字，消耗之并获得差值绝对值的力量。";
        c->imagePath = "";
        c->effects.push_back(
            Effect(std::make_unique<ConsumeLastTwoDigitsStrengthAction>())
            );
        c->isAttack = false;
        m_cards[c->id] = std::move(c);
    }
    // ── 测试易伤 ──
    {
        auto c = std::make_unique<CardData>();
        c->id   = "test_vulnerable";
        c->name = "测试易伤";
        c->cost = 0;
        c->desc = "给敌人施加 2 回合易伤";
        c->effects.push_back(Effect(std::make_unique<ApplyStatusAction>(
            StatusType::Vulnerable, 1, 2, ApplyStatusAction::Enemy)));
        m_cards[c->id] = std::move(c);
    }
    // ── 测试虚弱 ──
    {
        auto c = std::make_unique<CardData>();
        c->id   = "test_weak";
        c->name = "测试虚弱";
        c->cost = 0;
        c->desc = "给敌人施加 2 回合虚弱";
        c->effects.push_back(Effect(std::make_unique<ApplyStatusAction>(
            StatusType::Weak, 1, 2, ApplyStatusAction::Enemy)));
        m_cards[c->id] = std::move(c);
    }
    // ── 能力牌：每回合写入随机字母 ──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "random_letter_ability";
        c->name      = "随机触碰";
        c->cost      = 1;
        c->desc      = "获得能力：每回合开始时向字符串空间写入一个随机字母（永久）";
        c->imagePath = "";
        c->effects.push_back(
            Effect(std::make_unique<AddAbilityAction>("random_letter_start", 0, AddAbilityAction::Self))
            );
        c->exhaust = true;
        c->isAttack = false;
        m_cards[c->id] = std::move(c);
    }
    // ── 能力牌：写入时加格挡 ──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "block_on_write_ability";
        c->name      = "护符共鸣";
        c->cost      = 1;
        c->desc      = "获得能力：每次向字符串空间写入内容时，获得3点格挡（永久）";
        c->imagePath = "";
        c->effects.push_back(
            Effect(std::make_unique<AddAbilityAction>("block_on_write", 0, AddAbilityAction::Self))
            );
        c->exhaust = true;
        c->isAttack = false;
        m_cards[c->id] = std::move(c);
    }
    // ── 技能牌：本回合每打出一张牌造成5伤害 ──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "temp_damage_per_card";
        c->name      = "裂片爆发";
        c->cost      = 2;   // 可调
        c->desc      = "本回合每当你打出一张牌，对敌人造成5点伤害";
        c->imagePath = "";
        c->effects.push_back(
            Effect(std::make_unique<AddAbilityAction>("temp_damage_per_card", 1, AddAbilityAction::Self))
            );
        c->isAttack = false;
        m_cards[c->id] = std::move(c);
    }
    // ── 能力牌：专注 ──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "focus";
        c->name      = "专注";
        c->cost      = 1;               // 可按设计调整
        c->desc      = "永久将最大能量提升至 4，并回满能量。（消耗）";
        c->imagePath = "";
        c->effects.push_back(
            Effect(std::make_unique<SetMaxEnergyAction>(4))
            );
        c->exhaust = true;
        c->isAttack = false;
        m_cards[c->id] = std::move(c);
    }
    // ── 回费牌：充能脉冲 ──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "energy_pulse";
        c->name      = "充能脉冲";
        c->cost      = 0;
        c->desc      = "若字符串空间长度 ≤ 2，回复 2 点能量并写入 \"000\"。";
        c->imagePath = "";
        c->effects.push_back(
            Effect(std::make_unique<StringLengthLeCondition>(2),
                   std::make_unique<GainEnergyAction>(2))
            );
        c->effects.push_back(
            Effect(std::make_unique<StringLengthLeCondition>(2),
                   std::make_unique<WriteStringAction>("000", true))
            );
        c->isAttack = false;
        m_cards[c->id] = std::move(c);
    }
    // ── 抽牌：灵光一现 ──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "flash";
        c->name      = "灵光一现";
        c->cost      = 1;
        c->desc      = "抽 2 张牌。";
        c->imagePath = "";
        c->effects.push_back(
            Effect(std::make_unique<DrawAction>(2))
            );
        c->isAttack = false;
        m_cards[c->id] = std::move(c);
    }
    // ── 灵巧抽牌 ──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "swift_draw";
        c->name      = "灵巧抽牌";
        c->cost      = 0;
        c->desc      = "抽1张牌。若字符串空间长度 ≤ 3，造成 2×(3-长度) 点伤害。";
        c->effects.push_back(
            Effect(std::make_unique<DrawAndConditionalDamageAction>(1, 2, 3))
            );
        c->isAttack = true;   // 可能造成伤害，设为攻击牌
        m_cards[c->id] = std::move(c);
    }
    // ── 混沌瓦解 ──
    {
        auto c = std::make_unique<CardData>();
        c->id        = "chaos_break";
        c->name      = "混沌瓦解";
        c->cost      = 1;
        c->desc      = "移除所有字符，造成每字符0~5点随机伤害。若字母多于数字，移除自身所有格挡；若数字多于字母，自身受到总伤害一半。";
        c->effects.push_back(
            Effect(std::make_unique<ChaosRemoveAction>(0, 5))
            );
        c->isAttack = true;
        m_cards[c->id] = std::move(c);
    }
}
const CardData* CardDatabase::cardById(const QString& id) const {
    auto it = m_cards.find(id);
    if (it != m_cards.end()) {
        return it->second.get();   // unordered_map: it->second 是 unique_ptr&
    }
    qWarning() << "CardData not found:" << id;
    return nullptr;
}
