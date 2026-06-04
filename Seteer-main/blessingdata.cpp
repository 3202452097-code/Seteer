#include "blessingdata.h"
#include "EffectAction.h"
BlessingDatabase& BlessingDatabase::instance() {
    static BlessingDatabase db;
    return db;
}
BlessingDatabase::BlessingDatabase() { registerDefaults(); }
void BlessingDatabase::registerDefaults() {
    // ── 战争祈福：每回合 +2 力量 ──
    {
        auto b = std::make_unique<BlessingData>();
        b->id   = "war_blessing";
        b->name = "战争祈福";
        b->desc = "每回合开始时获得 2 点力量";
        b->effects.push_back(
            Effect(std::make_unique<GainStrengthAction>(2))
            );
        m_blessings[b->id] = std::move(b);
    }
    // ── 智慧祈福：每回合抽 1 牌 ──
    {
        auto b = std::make_unique<BlessingData>();
        b->id   = "wisdom_blessing";
        b->name = "智慧祈福";
        b->desc = "每回合开始时抽 1 张牌";
        b->effects.push_back(
            Effect(std::make_unique<DrawAction>(1))
            );
        m_blessings[b->id] = std::move(b);
    }
    // ── 坚韧祈福：每回合 +3 格挡 ──
    {
        auto b = std::make_unique<BlessingData>();
        b->id   = "tough_blessing";
        b->name = "坚韧祈福";
        b->desc = "每回合开始时获得 3 点格挡";
        b->effects.push_back(
            Effect(std::make_unique<BlockAction>(EffectValue::fixed(3)))
            );
        m_blessings[b->id] = std::move(b);
    }
}
const BlessingData* BlessingDatabase::blessingById(const QString& id) const {
    auto it = m_blessings.find(id);
    return (it != m_blessings.end()) ? it->second.get() : nullptr;
}
QList<QString> BlessingDatabase::allIds() const {
    QList<QString> ids;
    for (auto& pair : m_blessings) ids.append(pair.first);
    return ids;
}
