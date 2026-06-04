#ifndef EFFECTVALUE_H
#define EFFECTVALUE_H
class BattleContext; // 前向声明
struct EffectValue {
    enum Source {
        Fixed,            // 固定值 base
        StringLength,     // 字符串空间长度
        PlayerHP,
        PlayerMaxHP,
        PlayerBlock,
        EnemyHP,
        EnemyMaxHP,
        EnemyBlock,
        HandCount,
        DrawPileCount,
        DiscardPileCount,
    };
    Source source = Fixed;
    int base = 0;
    double multiplier = 1.0;
    int bonus = 0;
    // 核心：从战场上下文计算实际数值
    int evaluate(const BattleContext& ctx) const;
    // ── 工厂方法 ──
    static EffectValue fixed(int v) {
        return { Fixed, v, 1.0, 0 };
    }
    static EffectValue stringLength(double mult = 1.0, int bonusV = 0) {
        return { StringLength, 0, mult, bonusV };
    }
    static EffectValue playerHP(double mult = 1.0) {
        return { PlayerHP, 0, mult, 0 };
    }
    static EffectValue enemyBlock(double mult = 1.0, int bonusV = 0) {
        return { EnemyBlock, 0, mult, bonusV };
    }
    static EffectValue handCount(double mult = 1.0) {
        return { HandCount, 0, mult, 0 };
    }
};
#endif // EFFECTVALUE_H