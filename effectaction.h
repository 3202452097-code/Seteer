#ifndef EFFECTACTION_H
#define EFFECTACTION_H
#include <QString>
#include "EffectValue.h"
#include "Status.h"
#include <QDebug>
class BattleContext;
// 效果行为基类
class EffectAction {
public:
    virtual ~EffectAction() = default;
    virtual void execute(BattleContext& ctx) = 0;
    virtual QString description() const = 0;
};
// ── 示例：伤害 ──
class DamageAction : public EffectAction {
public:
    explicit DamageAction(EffectValue amount) : m_amount(amount) {}
    void execute(BattleContext& ctx) override;
    QString description() const override;
private:
    EffectValue m_amount;
};
// ── 示例：格挡 ──
class BlockAction : public EffectAction {
public:
    explicit BlockAction(EffectValue amount) : m_amount(amount) {}
    void execute(BattleContext& ctx) override;
    QString description() const override;
private:
    EffectValue m_amount;
};
// ── 示例：抽牌 ──
class DrawAction : public EffectAction {
public:
    explicit DrawAction(int count) : m_count(count) {}
    void execute(BattleContext& ctx) override;
    QString description() const override;
private:
    int m_count;
};
// ── 示例：写入字符串空间 ──
class WriteStringAction : public EffectAction {
public:
    WriteStringAction(QString text, bool append = true)
        : m_text(text), m_append(append) {}
    void execute(BattleContext& ctx) override;
    QString description() const override;
private:
    QString m_text;
    bool m_append;
};
// ── 示例：清空字符串空间 ──
class ClearStringAction : public EffectAction {
public:
    void execute(BattleContext& ctx) override;
    QString description() const override;
};
// ── ★ 新增：写入随机字母 ──
class WriteRandomLetterAction : public EffectAction {
public:
    void execute(BattleContext& ctx) override;
    QString description() const override;
};
// ── ★ 新增：写入随机数字 ──
class WriteRandomDigitAction : public EffectAction {
public:
    void execute(BattleContext& ctx) override;
    QString description() const override;
};
// ── ★ 新增：消耗最后数字字符 → 造成伤害 ──
class ConsumeLastDigitDamageAction : public EffectAction {
public:
    void execute(BattleContext& ctx) override;
    QString description() const override;
};
// ── ★ 新增：消耗最后字母字符 → 获得格挡 ──
class ConsumeLastAlphaBlockAction : public EffectAction {
public:
    explicit ConsumeLastAlphaBlockAction(int blockAmount = 5)
        : m_blockAmount(blockAmount) {}
    void execute(BattleContext& ctx) override;
    QString description() const override;
private:
    int m_blockAmount;
};
class ClearStringDamageAction : public EffectAction {
public:
    void execute(BattleContext& ctx) override;
    QString description() const override;
};
// ── ★ 获得力量 ──
class GainStrengthAction : public EffectAction {
public:
    explicit GainStrengthAction(int amount) : m_amount(amount) {}
    void execute(BattleContext& ctx) override;
    QString description() const override;
private:
    int m_amount;
};
// ── ★ 回复生命 ──
class HealAction : public EffectAction {
public:
    explicit HealAction(int amount) : m_amount(amount) {}
    void execute(BattleContext& ctx) override;
    QString description() const override;
private:
    int m_amount;
};
// ★ 消耗最后两个数字字符，获得差值力量
class ConsumeLastTwoDigitsStrengthAction : public EffectAction {
public:
    void execute(BattleContext& ctx) override;
    QString description() const override;
};
class ApplyStatusAction : public EffectAction {
public:
    enum Target { Enemy, Self };
    ApplyStatusAction(StatusType type, int amount, int duration, Target target)
        : m_type(type), m_amount(amount), m_duration(duration), m_target(target) {}
    void execute(BattleContext& ctx) override;
    QString description() const override;
private:
    StatusType m_type;
    int m_amount;
    int m_duration;
    Target m_target;
};
#endif // EFFECTACTION_H