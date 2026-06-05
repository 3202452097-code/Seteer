#include "Condition.h"
#include "BattleContext.h"

bool LastCharIsDigitCondition::evaluate(const BattleContext& ctx) const {
    return ctx.stringSpace->isLastDigit();
}
bool LastCharIsAlphaCondition::evaluate(const BattleContext& ctx) const {
    return ctx.stringSpace->isLastAlpha();
}
bool StringLengthLeCondition::evaluate(const BattleContext& ctx) const {
    return ctx.stringSpace->length() <= m_maxLength;
}



bool PlayerBlockZeroCondition::evaluate(const BattleContext& ctx) const {
    return ctx.player && ctx.player->block() == 0;
}
bool SelfHpNotFullCondition::evaluate(const BattleContext& ctx) const {
    if (!ctx.attacker) return false;
    return ctx.attacker->hp() < ctx.attacker->maxHp();
}