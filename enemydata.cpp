#include "EnemyData.h"
#include <random>

EnemyDatabase& EnemyDatabase::instance() {
    static EnemyDatabase db;
    return db;
}

EnemyDatabase::EnemyDatabase() {
    registerDefaults();
}

void EnemyDatabase::registerDefaults() {
    // ═══════════════════════════════════════
    // Layer 1 — 初级敌人
    // ═══════════════════════════════════════

    // 史莱姆：每回合攻击 5
    {
        auto e = std::make_unique<EnemyData>();
        e->id = "slime";
        e->name = "史莱姆";
        e->maxHp = 30;
        e->aiPattern = {{5, 0, 0, "攻击 5 点伤害"}};
        m_enemies[e->id] = std::move(e);
        m_layerPools[1].append("slime");
    }

    // 狼：攻击 6，每两回合获得 3 格挡
    {
        auto e = std::make_unique<EnemyData>();
        e->id = "wolf";
        e->name = "狼";
        e->maxHp = 35;
        e->aiPattern = {
                        {6, 0, 0, "攻击 6 点伤害"},
                        {6, 3, 0, "攻击 6 点 + 格挡 3"},
                        };
        m_enemies[e->id] = std::move(e);
        m_layerPools[1].append("wolf");
    }

    // 哥布林：每回合攻击 4 并提升 1 力量
    {
        auto e = std::make_unique<EnemyData>();
        e->id = "goblin";
        e->name = "哥布林";
        e->maxHp = 25;
        e->aiPattern = {{4, 0, 1, "攻击 4 点，力量 +1"}};
        m_enemies[e->id] = std::move(e);
        m_layerPools[1].append("goblin");
    }

    // ═══════════════════════════════════════
    // Layer 2 — 中级敌人
    // ═══════════════════════════════════════

    // 狂战士：三回合循环
    {
        auto e = std::make_unique<EnemyData>();
        e->id = "berserker";
        e->name = "狂战士";
        e->maxHp = 50;
        e->aiPattern = {
                        {8,  0, 0, "攻击 8 点"},
                        {10, 0, 0, "攻击 10 点"},
                        {0,  0, 2, "力量 +2"},
                        };
        m_enemies[e->id] = std::move(e);
        m_layerPools[2].append("berserker");
    }

    // 萨满：交替攻击和防御
    {
        auto e = std::make_unique<EnemyData>();
        e->id = "shaman";
        e->name = "萨满";
        e->maxHp = 45;
        e->aiPattern = {
                        {6, 4, 0, "攻击 6 点 + 格挡 4"},
                        {8, 0, 0, "攻击 8 点"},
                        };
        m_enemies[e->id] = std::move(e);
        m_layerPools[2].append("shaman");
    }

    // 骑士：攻击 7，每两回合格挡 5
    {
        auto e = std::make_unique<EnemyData>();
        e->id = "knight";
        e->name = "骑士";
        e->maxHp = 55;
        e->aiPattern = {
                        {7, 0, 0, "攻击 7 点"},
                        {7, 5, 0, "攻击 7 点 + 格挡 5"},
                        };
        m_enemies[e->id] = std::move(e);
        m_layerPools[2].append("knight");
    }

    // ═══════════════════════════════════════
    // Layer 3 — 高级敌人
    // ═══════════════════════════════════════

    // 恶魔：三回合循环
    {
        auto e = std::make_unique<EnemyData>();
        e->id = "demon";
        e->name = "恶魔";
        e->maxHp = 65;
        e->aiPattern = {
                        {10, 0, 0, "攻击 10 点"},
                        {12, 0, 3, "攻击 12 点 + 力量 +3"},
                        {8,  0, 0, "攻击 8 点"},
                        };
        m_enemies[e->id] = std::move(e);
        m_layerPools[3].append("demon");
    }

    // 火龙：高伤害 + 格挡
    {
        auto e = std::make_unique<EnemyData>();
        e->id = "dragon";
        e->name = "火龙";
        e->maxHp = 70;
        e->aiPattern = {
                        {9,  0, 0, "攻击 9 点"},
                        {15, 0, 0, "攻击 15 点"},
                        {0,  6, 0, "格挡 6"},
                        };
        m_enemies[e->id] = std::move(e);
        m_layerPools[3].append("dragon");
    }

    // 巫妖：攻击 + 格挡 + 力量混合
    {
        auto e = std::make_unique<EnemyData>();
        e->id = "lich";
        e->name = "巫妖";
        e->maxHp = 60;
        e->aiPattern = {
                        {7,  5, 0, "攻击 7 点 + 格挡 5"},
                        {12, 0, 0, "攻击 12 点"},
                        {0,  5, 3, "格挡 5 + 力量 +3"},
                        };
        m_enemies[e->id] = std::move(e);
        m_layerPools[3].append("lich");
    }

    // ═══════════════════════════════════════
    // Layer 4 — Boss
    // ═══════════════════════════════════════

    // 龙皇
    {
        auto e = std::make_unique<EnemyData>();
        e->id = "boss_dragon";
        e->name = "龙皇";
        e->maxHp = 100;
        e->aiPattern = {
                        {12, 0,  0, "攻击 12 点"},
                        {18, 0,  0, "攻击 18 点"},
                        {0,  0,  4, "力量 +4"},
                        {0,  10, 0, "格挡 10"},
                        };
        m_enemies[e->id] = std::move(e);
        m_layerPools[4].append("boss_dragon");
    }

    // 巫妖王
    {
        auto e = std::make_unique<EnemyData>();
        e->id = "boss_lich";
        e->name = "巫妖王";
        e->maxHp = 90;
        e->aiPattern = {
                        {10, 0,  3, "攻击 10 点 + 力量 +3"},
                        {8,  8,  0, "攻击 8 点 + 格挡 8"},
                        {14, 0,  0, "攻击 14 点"},
                        {0,  0,  5, "力量 +5"},
                        };
        m_enemies[e->id] = std::move(e);
        m_layerPools[4].append("boss_lich");
    }

    // 骑士王
    {
        auto e = std::make_unique<EnemyData>();
        e->id = "boss_knight";
        e->name = "骑士王";
        e->maxHp = 110;
        e->aiPattern = {
                        {8,  10, 0, "攻击 8 点 + 格挡 10"},
                        {14, 0,  0, "攻击 14 点"},
                        {0,  0,  3, "力量 +3"},
                        {0,  6,  0, "格挡 6"},
                        };
        m_enemies[e->id] = std::move(e);
        m_layerPools[4].append("boss_knight");
    }
}

const EnemyData* EnemyDatabase::enemyById(const QString& id) const {
    auto it = m_enemies.find(id);
    return (it != m_enemies.end()) ? it->second.get() : nullptr;
}

QList<QString> EnemyDatabase::enemyIdsForLayer(int layer) const {
    auto it = m_layerPools.find(layer);
    return (it != m_layerPools.end()) ? it->second : QList<QString>{};
}

QString EnemyDatabase::randomEnemyForLayer(int layer) const {
    QList<QString> pool = enemyIdsForLayer(layer);
    if (pool.isEmpty()) return "";
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(pool.begin(), pool.end(), g);
    return pool.first();
}