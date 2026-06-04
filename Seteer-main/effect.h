#ifndef EFFECT_H
#define EFFECT_H
#include <memory>
#include "condition.h"
#include "effectaction.h"
// 一个效果 = 可选条件 + 必须行为
struct Effect {
    std::unique_ptr<Condition> condition;   // nullptr = 无条件
    std::unique_ptr<EffectAction> action;
    // 默认构造
    Effect() = default;
    // 便捷构造：无条件
    Effect(std::unique_ptr<EffectAction> act)
        : condition(nullptr), action(std::move(act)) {}
    // 便捷构造：有条件
    Effect(std::unique_ptr<Condition> cond, std::unique_ptr<EffectAction> act)
        : condition(std::move(cond)), action(std::move(act)) {}
    // 只移动，不拷贝
    Effect(Effect&&) = default;
    Effect& operator=(Effect&&) = default;
    void execute(BattleContext& ctx) const {
        if (!condition || condition->evaluate(ctx) != condition->invert) {
            action->execute(ctx);
        }
    }
};
#endif // EFFECT_H
