#include "enemydata.h"
#include <random>

EnemyDatabase& EnemyDatabase::instance() {
    static EnemyDatabase db;
    return db;
}

EnemyDatabase::EnemyDatabase() { registerDefaults(); }

void EnemyDatabase::registerDefaults() {
    // ═══════════════════════════════════
    // Layer 1 - 小怪
    // ═══════════════════════════════════
    m_factories["doubao"]   = []() { return std::make_unique<Doubao>(); };
    m_factories["hajimi"]   = []() { return std::make_unique<Hajimi>(); };
    m_factories["freshman"] = []() { return std::make_unique<Freshman>(); };
    m_factories["python"]   = []() { return std::make_unique<Python>(); };

    m_layerPools[1] = {"doubao", "hajimi", "freshman", "python"};
    m_layerPools[2] = {"doubao", "hajimi", "freshman", "python"};

    // ═══════════════════════════════════
    // Layer 3 - 精英
    // ═══════════════════════════════════
    m_factories["admath"]         = []() { return std::make_unique<AdMath>(); };
    m_factories["linearalgebra"]  = []() { return std::make_unique<LinearAlgebra>(); };

    m_layerPools[3] = {"admath", "linearalgebra"};

    // ═══════════════════════════════════
    // Layer 4 - Boss
    // ═══════════════════════════════════
    m_factories["dragon"] = []() { return std::make_unique<Dragon>(); };
    m_factories["genius"] = []() { return std::make_unique<Genius>(); };

    m_layerPools[4] = {"dragon", "genius"};
}

std::unique_ptr<Enemy> EnemyDatabase::createEnemy(const QString& id) const {
    auto it = m_factories.find(id);
    return (it != m_factories.end()) ? it->second() : nullptr;
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

bool EnemyDatabase::hasEnemy(const QString& id) const {
    return m_factories.find(id) != m_factories.end();
}