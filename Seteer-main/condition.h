#ifndef CONDITION_H
#define CONDITION_H
class BattleContext;   // ★ 前向声明，不 include
class Condition {
public:
    virtual ~Condition() = default;
    virtual bool evaluate(const BattleContext& ctx) const = 0;
    bool invert = false;
};
// ★ 只声明，不实现
class LastCharIsDigitCondition : public Condition {
public:
    bool evaluate(const BattleContext& ctx) const override;
};
class LastCharIsAlphaCondition : public Condition {
public:
    bool evaluate(const BattleContext& ctx) const override;
};
class PlayerBlockZeroCondition : public Condition {
public:
    bool evaluate(const BattleContext& ctx) const override;
};
class SelfHpNotFullCondition : public Condition {
public:
    bool evaluate(const BattleContext& ctx) const override;
};
class StringLengthLeCondition : public Condition {
public:
    explicit StringLengthLeCondition(int maxLength) : m_maxLength(maxLength) {}
    bool evaluate(const BattleContext& ctx) const override;
private:
    int m_maxLength;
};
#endif // CONDITION_H