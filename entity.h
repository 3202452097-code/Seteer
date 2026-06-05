#ifndef ENTITY_H
#define ENTITY_H

#include <QString>
#include <QList>
#include <memory>
#include "Effect.h"
#include "Status.h"
#include "abilitydata.h"

class BattleContext;
struct AbilityInstance {
    QString abilityId;
    int remainingTurns = 0;   // 0=永久, >0表示剩余回合
};

// ==================== Entity 基类 ====================
class Entity {
public:
    Entity(int maxHp);
    virtual ~Entity() = default;

    int hp() const { return m_hp; }
    int maxHp() const { return m_maxHp; }
    int block() const { return m_block; }
    int strength() const { return m_strength; }
    bool isDead() const { return m_hp <= 0; }

    void setHP(int v);
    void setBlock(int v);
    void setStrength(int v) { m_strength = v; }
    void addStrength(int v) { m_strength += v; }
    void takeDamage(int dmg);
    std::function<void(int damage)> onDamaged;

    void addAbility(const QString& id, int duration = 0);
    bool hasAbility(const QString& id) const;
    void removeAbility(const QString& id);
    void tickAbilities();   // 每回合减少持续时间并移除到期能力
    void triggerAbilities(Trigger trigger, const QVariantMap& eventData, BattleContext& ctx);

    virtual void onTurnStart(BattleContext& ctx) {}
    virtual void onTurnEnd(BattleContext& ctx) {}
    virtual QString getName() const { return "敌人"; }

    // 状态
    void addStatus(StatusType type, int amount, int duration);
    int  getStatusAmount(StatusType type) const;
    bool hasStatus(StatusType type) const { return getStatusAmount(type) > 0; }
    void tickStatuses();
    const QList<StatusInstance>& statuses() const { return m_statuses; }
    const QList<AbilityInstance>& abilities() const { return m_abilities; }

protected:
    int m_hp;
    int m_maxHp;
    int m_block = 0;
    int m_strength = 0;
    QList<StatusInstance> m_statuses;
    QList<AbilityInstance> m_abilities;
};

// ==================== Player ====================
class Player : public Entity {
public:
    Player(int maxHp = 70, int maxEnergy = 3);
    int energy() const { return m_energy; }
    int maxEnergy() const { return m_maxEnergy; }
    void setEnergy(int v);
    void setMaxEnergy(int v) { m_maxEnergy = v; }
    void restoreEnergy();
    void onTurnStart(BattleContext& ctx) override;
protected:
    int m_energy;
    int m_maxEnergy;
};

// ==================== EnemyAI 基类 ====================
class EnemyAI {
public:
    virtual ~EnemyAI() = default;

    // ★ 返回本回合效果列表
    virtual std::vector<Effect> decide(const BattleContext& ctx) = 0;

    // 可选回调
    virtual void onTurnStart(BattleContext& ctx) {}
    virtual void onTurnEnd(BattleContext& ctx) {}
    virtual void executeSpecialEffect(BattleContext& ctx, int code) {}

    // 意图描述（decide 后读取）
    QString intentDescription() const { return m_intentDesc; }

    // 特殊效果码（decide 可能设置）
    int specialEffectCode() const { return m_specialCode; }

protected:
    QString m_intentDesc;
    int m_specialCode = 0;
};

// ── 小怪 AI ──
class DoubaoAI : public EnemyAI {
public: std::vector<Effect> decide(const BattleContext& ctx) override;
private: int m_turn = 0;
};

class HajimiAI : public EnemyAI {
public: std::vector<Effect> decide(const BattleContext& ctx) override;
private: int m_turn = 0;
};

class FreshmanAI : public EnemyAI {
public: std::vector<Effect> decide(const BattleContext& ctx) override;
private: int m_turn = 0;
};

class PythonAI : public EnemyAI {
public: std::vector<Effect> decide(const BattleContext& ctx) override;
private: int m_turn = 0;
};

// ── 精英 AI ──
class AdMathAI : public EnemyAI {
public: std::vector<Effect> decide(const BattleContext& ctx) override;
private: int m_turn = 0;
};

class LinearAlgebraAI : public EnemyAI {
public: std::vector<Effect> decide(const BattleContext& ctx) override;
    void executeSpecialEffect(BattleContext& ctx, int code) override;
private: int m_turn = 0;
};

// ── Boss AI ──
class DragonAI : public EnemyAI {
public: std::vector<Effect> decide(const BattleContext& ctx) override;
private: int m_turn = 0; int m_phase = 1;
};

class GeniusAI : public EnemyAI {
public: std::vector<Effect> decide(const BattleContext& ctx) override;
private: int m_turn = 0; bool m_enraged = false;
};

// ==================== Enemy ====================
class Enemy : public Entity {
public:
    Enemy(int maxHp, std::unique_ptr<EnemyAI> ai = nullptr);
    virtual ~Enemy() = default;

    // 意图
    std::vector<Effect> takePendingEffects();       // 取出并清空
    QString intentDescription() const { return m_intentDescription; }
    int intentSpecialEffect() const { return m_specialCode; }

    void setAI(std::unique_ptr<EnemyAI> ai);
    void decideIntent(BattleContext& ctx);
    void executeSpecialEffect(BattleContext& ctx);
    void onTurnStart(BattleContext& ctx) override;
    void onTurnEnd(BattleContext& ctx) override;

    EnemyAI* getAI() const { return m_ai.get(); }
    QString getName() const override { return m_name; }
    void setName(const QString& name) { m_name = name; }

protected:
    std::vector<Effect> m_pendingEffects;
    std::unique_ptr<EnemyAI> m_ai;
    QString m_name = "敌人";
    QString m_intentDescription;
    int m_specialCode = 0;
};

// ── 小怪派生类 ──
class Doubao : public Enemy {
public: Doubao(int hp = 35);
};
class Hajimi : public Enemy {
public: Hajimi(int hp = 30);
};
class Freshman : public Enemy {
public: Freshman(int hp = 40);
};
class Python : public Enemy {
public: Python(int hp = 25);
};

// ── 精英派生类 ──
class AdMath : public Enemy {
public: AdMath(int hp = 70);
};
class LinearAlgebra : public Enemy {
public: LinearAlgebra(int hp = 60);
};

// ── Boss 派生类 ──
class Dragon : public Enemy {
public: Dragon(int hp = 140);
};
class Genius : public Enemy {
public: Genius(int hp = 120);
};

#endif // ENTITY_H