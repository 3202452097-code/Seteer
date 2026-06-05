#include "entity.h"
#include "BattleContext.h"
#include "effectaction.h"
#include <QDebug>

// ==================== Entity ====================
Entity::Entity(int maxHp) : m_hp(maxHp), m_maxHp(maxHp) {}

void Entity::setHP(int v)   { m_hp = qMax(0, qMin(v, m_maxHp)); }
void Entity::setBlock(int v) { m_block = qMax(0, v); }

void Entity::takeDamage(int dmg) {
    if (dmg <= 0 || isDead()) return;

    if (hasStatus(StatusType::Vulnerable)) {
        dmg = static_cast<int>(dmg * 1.5);
    }

    if (m_block > 0) {
        int blocked = qMin(dmg, m_block);
        m_block -= blocked;
        dmg -= blocked;
    }
    m_hp = qMax(0, m_hp - dmg);
    if (onDamaged && dmg > 0) onDamaged(dmg);
    if (onHit     && dmg > 0) onHit(dmg);
}

// ==================== Status ====================
void Entity::addStatus(StatusType type, int amount, int duration) {
    for (auto& s : m_statuses) {
        if (s.type == type) {
            s.amount = amount;
            s.duration += duration;
            return;
        }
    }
    m_statuses.append({type, amount, duration});
}

int Entity::getStatusAmount(StatusType type) const {
    for (const auto& s : m_statuses) {
        if (s.type == type) return s.amount;
    }
    return 0;
}

void Entity::tickStatuses() {
    for (int i = m_statuses.size() - 1; i >= 0; i--) {
        m_statuses[i].duration--;
        if (m_statuses[i].duration <= 0) m_statuses.removeAt(i);
    }
}

void Entity::addAbility(const QString& id, int duration) {
    const AbilityData* data = AbilityDatabase::instance().abilityById(id);
    if (!data) return;
    // 如果已经存在，则刷新持续时间（默认新持续时间覆盖？）
    for (auto& inst : m_abilities) {
        if (inst.abilityId == id) {
            // 如果新的持续时间更长或永久，覆盖；否则保留较长的
            if (duration == 0 || (inst.remainingTurns > 0 && duration > inst.remainingTurns)) {
                inst.remainingTurns = duration;
            } else if (inst.remainingTurns == 0) {
                // 已永久，不做改变
            } else {
                inst.remainingTurns = qMax(inst.remainingTurns, duration);
            }
            return;
        }
    }
    m_abilities.append({id, duration});
}
bool Entity::hasAbility(const QString& id) const {
    for (const auto& inst : m_abilities) {
        if (inst.abilityId == id) return true;
    }
    return false;
}
void Entity::removeAbility(const QString& id) {
    for (int i = m_abilities.size() - 1; i >= 0; i--) {
        if (m_abilities[i].abilityId == id) {
            m_abilities.removeAt(i);
        }
    }
}
void Entity::tickAbilities() {
    for (int i = m_abilities.size() - 1; i >= 0; i--) {
        if (m_abilities[i].remainingTurns > 0) {
            m_abilities[i].remainingTurns--;
            if (m_abilities[i].remainingTurns <= 0) {
                m_abilities.removeAt(i);
            }
        }
    }
}
void Entity::triggerAbilities(Trigger trigger, const QVariantMap& eventData, BattleContext& ctx) {
    for (int i = m_abilities.size() - 1; i >= 0; i--) {
        const AbilityData* data = AbilityDatabase::instance().abilityById(m_abilities[i].abilityId);
        if (data && data->trigger == trigger) {
            // 执行前临时设置 attacker/defender (已由调用者设置好)
            data->action->execute(ctx);
        }
    }
}

// ==================== Player ====================
Player::Player(int maxHp, int maxEnergy)
    : Entity(maxHp), m_energy(maxEnergy), m_maxEnergy(maxEnergy) {}
void Player::setEnergy(int v) { m_energy = qMax(0, qMin(v, m_maxEnergy)); }
void Player::restoreEnergy()   { m_energy = m_maxEnergy; }
void Player::onTurnStart(BattleContext&) {
    restoreEnergy();
    setBlock(0);
}

// ═══════════════════════════════════
// 小怪 AI
// ═══════════════════════════════════

std::vector<Effect> DoubaoAI::decide(const BattleContext&) {
    m_turn++;
    std::vector<Effect> effects;
    switch (m_turn % 3) {
    case 1:
        effects.push_back(Effect(std::make_unique<DamageAction>(EffectValue::fixed(5))));
        m_intentDesc = "认知偏差：造成 5 点伤害";
        break;
    case 2:
        effects.push_back(Effect(std::make_unique<BlockAction>(EffectValue::fixed(8))));
        m_intentDesc = "深度学习：获得 8 点格挡";
        break;
    case 0:
        effects.push_back(Effect(std::make_unique<GainStrengthAction>(1)));
        m_intentDesc = "创造性AI：获得 1 点力量";
        break;
    }
    return effects;
}

std::vector<Effect> HajimiAI::decide(const BattleContext&) {
    m_turn++;
    std::vector<Effect> effects;
    if (m_turn % 2 == 0) {
        effects.push_back(Effect(std::make_unique<DamageAction>(EffectValue::fixed(8))));
        m_intentDesc = "哈！！：造成 8 点伤害";
    } else {
        effects.push_back(Effect(std::make_unique<GainStrengthAction>(2)));
        m_intentDesc = "脊背龙模式：获得 2 点力量";
    }
    return effects;
}

std::vector<Effect> FreshmanAI::decide(const BattleContext&) {
    m_turn++;
    std::vector<Effect> effects;
    switch (m_turn % 4) {
    case 1:
        effects.push_back(Effect(std::make_unique<DamageAction>(EffectValue::fixed(6))));
        m_intentDesc = "投掷水杯：造成 6 点伤害";
        break;
    case 2:
        effects.push_back(Effect(std::make_unique<GainStrengthAction>(3)));
        m_intentDesc = "生命体征维持餐：获得 3 点力量";
        break;
    case 3:
        effects.push_back(Effect(std::make_unique<DamageAction>(EffectValue::fixed(10))));
        m_intentDesc = "北大拳：造成 10 点伤害";
        break;
    case 0:
        effects.push_back(Effect(std::make_unique<BlockAction>(EffectValue::fixed(5))));
        m_intentDesc = "闪：获得 5 点格挡";
        break;
    }
    return effects;
}

std::vector<Effect> PythonAI::decide(const BattleContext&) {
    m_turn++;
    std::vector<Effect> effects;
    if (m_turn % 2 == 0) {
        effects.push_back(Effect(std::make_unique<BlockAction>(EffectValue::fixed(6))));
        effects.push_back(Effect(std::make_unique<DamageAction>(EffectValue::fixed(4))));
        m_intentDesc = "import numpy as np：造成 4 点伤害并获6格挡";
    } else {
        effects.push_back(Effect(std::make_unique<DamageAction>(EffectValue::fixed(7))));
        m_intentDesc = "孩子们我的库好用吗：造成 7 点伤害";
    }
    return effects;
}

// ═══════════════════════════════════
// 精英 AI
// ═══════════════════════════════════

std::vector<Effect> AdMathAI::decide(const BattleContext&) {
    m_turn++;
    std::vector<Effect> effects;
    if (m_turn == 1) {
        effects.push_back(Effect(std::make_unique<BlockAction>(EffectValue::fixed(10))));
        m_intentDesc = "斯托克斯公式：获得 10 点格挡";
    } else if (m_turn % 3 == 0) {
        effects.push_back(Effect(std::make_unique<DamageAction>(EffectValue::fixed(14))));
        m_intentDesc = "泰勒展开：造成 14 点伤害";
    } else {
        effects.push_back(Effect(std::make_unique<DamageAction>(EffectValue::fixed(9))));
        m_intentDesc = "柯西收敛：造成 9 点伤害";
    }
    return effects;
}

std::vector<Effect> LinearAlgebraAI::decide(const BattleContext&) {
    m_turn++;
    std::vector<Effect> effects;
    m_specialCode = 0;
    if (m_turn % 3 == 1) {
        effects.push_back(Effect(std::make_unique<DamageAction>(EffectValue::fixed(8))));
        m_specialCode = 1;  // 诅咒
        m_intentDesc = "转置：造成 8 点伤害，附带诅咒";
    } else if (m_turn % 3 == 2) {
        effects.push_back(Effect(std::make_unique<BlockAction>(EffectValue::fixed(8))));
        m_intentDesc = "求Jordan标准型：获得 8 点格挡";
    } else {
        effects.push_back(Effect(std::make_unique<GainStrengthAction>(2)));
        m_intentDesc = "升维：获得 2 点力量";
    }
    return effects;
}

void LinearAlgebraAI::executeSpecialEffect(BattleContext& ctx, int code) {
    if (code == 1 && ctx.player) { ctx.player->addStrength(-1); qDebug() << "[线性代数] 玩家力量-1"; }
}

// ═══════════════════════════════════
// Boss AI
// ═══════════════════════════════════

std::vector<Effect> DragonAI::decide(const BattleContext& ctx) {
    m_turn++;
    std::vector<Effect> effects;
    if (ctx.enemy && ctx.enemy->hp() < ctx.enemy->maxHp() / 2) m_phase = 2;

    if (m_phase == 1) {
        switch (m_turn % 3) {
        case 1:
            effects.push_back(Effect(std::make_unique<DamageAction>(EffectValue::fixed(12))));
            m_intentDesc = "龙息：造成 12 点伤害";
            break;
        case 2:
            effects.push_back(Effect(std::make_unique<BlockAction>(EffectValue::fixed(12))));
            m_intentDesc = "龙鳞护甲：获得 12 点格挡";
            break;
        case 0:
            effects.push_back(Effect(std::make_unique<GainStrengthAction>(3)));
            m_intentDesc = "召唤考试周：获得 3 点力量";
            break;
        }
    } else {
        if (m_turn % 2 == 0) {
            effects.push_back(Effect(std::make_unique<DamageAction>(EffectValue::fixed(18))));
            m_intentDesc = "挂科风暴：造成 18 点伤害";
        } else {
            effects.push_back(Effect(std::make_unique<GainStrengthAction>(4)));
            m_intentDesc = "龙怒：获得 4 点力量";
        }
    }
    return effects;
}

int DragonAI::currentPhase() const {
    return m_phase;
}


std::vector<Effect> GeniusAI::decide(const BattleContext& ctx) {
    m_turn++;
    std::vector<Effect> effects;
    if (!m_enraged && ctx.enemy && ctx.enemy->hp() < ctx.enemy->maxHp() / 3) {
        m_enraged = true; qDebug() << "[程设大佬] 进入狂暴状态！";
    }
    if (m_enraged) {
        effects.push_back(Effect(std::make_unique<DamageAction>(EffectValue::fixed(16))));
        m_intentDesc = "事后道歉：造成 16 点伤害";
    } else {
        switch (m_turn % 3) {
        case 1:
            effects.push_back(Effect(std::make_unique<DamageAction>(EffectValue::fixed(10))));
            m_intentDesc = "10道C++上机题热身：造成 10 点伤害";
            break;
        case 2:
            effects.push_back(Effect(std::make_unique<BlockAction>(EffectValue::fixed(10))));
            m_intentDesc = "手搓MySTL：获得 10 点格挡";
            break;
        case 0:
            effects.push_back(Effect(std::make_unique<GainStrengthAction>(2)));
            m_intentDesc = "派生子类：获得 2 点力量";
            break;
        }
    }
    return effects;
}

// ==================== Enemy ====================
Enemy::Enemy(int maxHp, std::unique_ptr<EnemyAI> ai)
    : Entity(maxHp), m_ai(std::move(ai)) {}

void Enemy::setAI(std::unique_ptr<EnemyAI> ai) { m_ai = std::move(ai); }

std::vector<Effect> Enemy::takePendingEffects() {
    std::vector<Effect> result = std::move(m_pendingEffects);
    m_pendingEffects.clear();
    return result;
}

void Enemy::decideIntent(BattleContext& ctx) {
    if (m_ai) {
        m_pendingEffects = m_ai->decide(ctx);
        m_intentDescription = m_ai->intentDescription();
        m_specialCode = m_ai->specialEffectCode();
    }
}

void Enemy::executeSpecialEffect(BattleContext& ctx) {
    if (m_ai && m_specialCode != 0) {
        m_ai->executeSpecialEffect(ctx, m_specialCode);
    }
}

void Enemy::onTurnStart(BattleContext& ctx) {
    setBlock(0);
    if (m_ai) m_ai->onTurnStart(ctx);
}

void Enemy::onTurnEnd(BattleContext& ctx) {
    if (m_ai) m_ai->onTurnEnd(ctx);
}

// ==================== 派生类构造 ====================
Doubao::Doubao(int hp)          : Enemy(hp, std::make_unique<DoubaoAI>())       { setName("豆包"); }
Hajimi::Hajimi(int hp)          : Enemy(hp, std::make_unique<HajimiAI>())      { setName("一只哈基米"); }
Freshman::Freshman(int hp)      : Enemy(hp, std::make_unique<FreshmanAI>())    { setName("一般路过的大一新生"); }
Python::Python(int hp)          : Enemy(hp, std::make_unique<PythonAI>())      { setName("一只叫Python的蟒蛇"); }

AdMath::AdMath(int hp)
    : Enemy(hp, std::make_unique<AdMathAI>())
{
    setName("高等数学");
    addAbility("admath_strength", 0);
}

LinearAlgebra::LinearAlgebra(int hp) : Enemy(hp, std::make_unique<LinearAlgebraAI>()) { setName("线性代数"); }

Dragon::Dragon(int hp)
    : Enemy(hp, std::make_unique<DragonAI>())
{
    setName("理教巨龙");
    addAbility("dragon_scales", 0);
    addAbility("dragon_regen", 0);
}

Genius::Genius(int hp)
    : Enemy(hp, std::make_unique<GeniusAI>())
{
    setName("程设大佬");
    addAbility("genius_break", 0);
}