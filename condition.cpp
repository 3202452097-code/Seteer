#include "Condition.h"
#include "BattleContext.h"
bool LastCharIsDigitCondition::evaluate(const BattleContext& ctx) const {
    return ctx.stringSpace->isLastDigit();
}
bool LastCharIsAlphaCondition::evaluate(const BattleContext& ctx) const {
    return ctx.stringSpace->isLastAlpha();
}