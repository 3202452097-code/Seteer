#include "effectvalue.h"
#include "BattleContext.h"
    int EffectValue::evaluate(const BattleContext& ctx) const {
    int raw = 0;
    switch (source) {
    case Fixed:            raw = base; break;
    case StringLength:     raw = ctx.stringSpace->length(); break;
    case PlayerHP:         raw = ctx.player->hp(); break;
    case PlayerMaxHP:      raw = ctx.player->maxHp(); break;
    case PlayerBlock:      raw = ctx.player->block(); break;
    case EnemyHP:          raw = ctx.enemy->hp(); break;
    case EnemyMaxHP:       raw = ctx.enemy->maxHp(); break;
    case EnemyBlock:       raw = ctx.enemy->block(); break;
    case HandCount:        raw = ctx.hand->size(); break;
    case DrawPileCount:    raw = ctx.drawPile->size(); break;
    case DiscardPileCount: raw = ctx.discardPile->size(); break;
    }
    return static_cast<int>(raw * multiplier) + bonus;
}