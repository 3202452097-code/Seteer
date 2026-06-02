#include "entity.h"
#include "BattleContext.h"
// ==================== Entity ====================
Entity::Entity(int maxHp) : m_hp(maxHp), m_maxHp(maxHp) {}
void Entity::setHP(int v)   { m_hp = qMax(0, qMin(v, m_maxHp)); }
void Entity::setBlock(int v) { m_block = qMax(0, v); }
void Entity::takeDamage(int dmg) {
    if (dmg <= 0 || isDead()) return;
    if (m_block > 0) {
        int blocked = qMin(dmg, m_block);
        m_block -= blocked;
        dmg -= blocked;
    }
    m_hp = qMax(0, m_hp - dmg);
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
// ==================== Enemy ====================
Enemy::Enemy(int maxHp, std::unique_ptr<EnemyAI> ai)
    : Entity(maxHp), m_ai(std::move(ai)) {}
void Enemy::setAI(std::unique_ptr<EnemyAI> ai) { m_ai = std::move(ai); }
void Enemy::decideIntent(BattleContext& ctx) {
    if (m_ai) {
        m_decision = m_ai->decide(ctx);
    }
}
void Enemy::onTurnStart(BattleContext& ctx) {
    setBlock(0);
    // ★ 执行决策中的非攻击行为（格挡 / 力量）
    if (m_decision.selfBlock > 0) {
        setBlock(block() + m_decision.selfBlock);
    }
    if (m_decision.strengthGain > 0) {
        addStrength(m_decision.strengthGain);
    }
    // 注意：不在这里调 decideIntent！意图在上回合结束时已决定
}
// ==================== SimpleAI ====================
SimpleAI::Decision SimpleAI::decide(const BattleContext&) {
    return { 6, 0, 0, "攻击 6 点伤害" };
}
// ==================== PatternAI ====================
PatternAI::Decision PatternAI::decide(const BattleContext&) {
    m_turn++;
    Decision d;
    switch (m_turn % 3) {
    case 1:
        d = { 6, 0, 0, "攻击 6 点伤害" };
        break;
    case 2:
        d = { 7, 5, 0, "攻击 7 点 + 自身格挡 5" };
        break;
    case 0:
        d = { 0, 0, 2, "力量 +2（不攻击）" };
        break;
    }
    return d;
}