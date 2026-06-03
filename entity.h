#ifndef ENTITY_H
#define ENTITY_H
#include <QString>
#include <QList>
#include <memory>
class BattleContext;
// ==================== 实体基类 ====================
class Entity {
public:
    Entity(int maxHp);
    virtual ~Entity() = default;
    int hp() const { return m_hp; }
    int maxHp() const { return m_maxHp; }
    int block() const { return m_block; }
    int strength() const {return m_strength; }
    bool isDead() const { return m_hp <= 0; }
    void setHP(int v);
    void setBlock(int v);
    void setStrength(int v) { m_strength = v; }
    void addStrength(int v) { m_strength += v; }
    void takeDamage(int dmg);       // 先扣格挡再扣血
    virtual void onTurnStart(BattleContext& ctx) {}
    virtual void onTurnEnd(BattleContext& ctx) {}
protected:
    int m_hp;
    int m_maxHp;
    int m_block = 0;
    int m_strength = 0;
};
// ==================== 玩家 ====================
class Player : public Entity {
public:
    Player(int maxHp = 70, int maxEnergy = 3);
    int energy() const { return m_energy; }
    int maxEnergy() const { return m_maxEnergy; }
    void setEnergy(int v);
    void restoreEnergy();
    void onTurnStart(BattleContext& ctx) override;
protected:
    int m_energy;
    int m_maxEnergy;
};
// ==================== 敌人 AI 基类 ====================
class EnemyAI {
public:
    virtual ~EnemyAI() = default;
    struct Decision {
        int damage = 0;
        int selfBlock = 0;
        int strengthGain = 0;
        QString description;
    };
    virtual Decision decide(const BattleContext& ctx) = 0;
};
// ── 简单 AI ──
class SimpleAI : public EnemyAI {
public:
    Decision decide(const BattleContext& ctx) override;
};
class PatternAI : public EnemyAI {
public:
    Decision decide(const BattleContext& ctx) override;
private:
    int m_turn = 0;
};
// ==================== 敌人 ====================
class Enemy : public Entity {
public:
    Enemy(int maxHp = 40, std::unique_ptr<EnemyAI> ai = nullptr);
    int intentDamage() const        { return m_decision.damage; }
    int intentSelfBlock() const     { return m_decision.selfBlock; }
    int intentStrengthGain() const  { return m_decision.strengthGain; }
    QString intentDescription() const { return m_decision.description; }
    void setAI(std::unique_ptr<EnemyAI> ai);
    void decideIntent(BattleContext& ctx);       // 决定本回合意图
    void onTurnStart(BattleContext& ctx) override;
protected:
    EnemyAI::Decision m_decision;
    std::unique_ptr<EnemyAI> m_ai;
};
class ConfigurableAI : public EnemyAI {
public:
    struct Step {
        int damage = 0;
        int selfBlock = 0;
        int strengthGain = 0;
        QString description;
    };
    explicit ConfigurableAI(const QList<Step>& steps);
    Decision decide(const BattleContext& ctx) override;
private:
    QList<Step> m_steps;
    int m_index = 0;
};

#endif // ENTITY_H