#include "runmanager.h"
#include "blessingdata.h"
#include "carddata.h"
#include "EnemyData.h"
#include <QRandomGenerator>
#include <algorithm>
#include <QDebug>

RunManager::RunManager(QObject* parent) : QObject(parent) {}

// ═══════════════════════════════════
// 默认牌组
// ═══════════════════════════════════
QList<Card> RunManager::defaultDeck() {
    QList<Card> deck;
    for (int i = 0; i < 4; i++) deck.append(Card{"strike"});
    for (int i = 0; i < 4; i++) deck.append(Card{"defend"});
    for (int i = 0; i < 1; i++) deck.append(Card{"random_letter"});
    for (int i = 0; i < 1; i++) deck.append(Card{"random_digit"});
    deck.append(Card{"clear_burst"});
    deck.append(Card{"triple_letter"});
    deck.append(Card{"digit_diff_strength"});
    deck.append(Card{"test_vulnerable"});
    deck.append(Card{"test_weak"});
    deck.append(Card{"random_letter_ability"});
    deck.append(Card{"focus"});
    deck.append(Card{"energy_pulse"});
    deck.append(Card{"flash"});
    deck.append(Card{"flash"});
    return deck;
}

// ═══════════════════════════════════
// Debug 接口
// ═══════════════════════════════════
void RunManager::setDebugEnemy(const QString& enemyId) {
    m_debugEnemy = enemyId;
}

void RunManager::setDebugInitialDeck(const QList<Card>& deck) {
    m_debugDeck = deck;
}

// ═══════════════════════════════════
// 启动
// ═══════════════════════════════════
void RunManager::start() {
    m_playerData = PlayerRunData{};
    m_floor = 0;

    // 牌组：debug 预设 > RunManager 默认
    if (!m_debugDeck.isEmpty()) {
        m_playerData.deck = m_debugDeck;
    } else {
        m_playerData.deck = defaultDeck();
    }

    // ═══════════════════════════════════
    // 楼层计划：一祈福 → 四层（每层一战一选牌一休息）→ 结束
    // ═══════════════════════════════════
    m_floorPlan.clear();
    m_floorPlan.append({FloorStep::Blessing, "", 0});

    for (int layer = 1; layer <= 4; layer++) {
        m_floorPlan.append({FloorStep::Battle, "", layer});
        if (layer < 4) {
            m_floorPlan.append({FloorStep::CardPick, "", 0});
            m_floorPlan.append({FloorStep::Rest, "", 0});
        }
    }
    m_floorPlan.append({FloorStep::End, "", 0});

    nextFloor();
}

// ═══════════════════════════════════
// 楼层推进
// ═══════════════════════════════════
void RunManager::nextFloor() {
    if (m_floor >= m_floorPlan.size() ||
        m_floorPlan[m_floor].type == FloorStep::End) {
        emit runFinished();
        return;
    }
    executeFloor(m_floorPlan[m_floor]);
}

// ═══════════════════════════════════
// 敌人选择
// ═══════════════════════════════════
QString RunManager::resolveEnemyId(const FloorStep& step) const {
    if (!m_debugEnemy.isEmpty()) {
        if (EnemyDatabase::instance().hasEnemy(m_debugEnemy)) {
            return m_debugEnemy;                    // 有效 ID
        }
        qWarning() << "Debug enemy not found:" << m_debugEnemy << ", fallback to random";
    }
    return EnemyDatabase::instance().randomEnemyForLayer(step.layer);
}

// ═══════════════════════════════════
// 执行楼层
// ═══════════════════════════════════
void RunManager::executeFloor(const FloorStep& step) {
    switch (step.type) {
    case FloorStep::Blessing: {
        QList<QString> ids = generateBlessingOptions();
        emit blessingOptionsAvailable(ids);
        break;
    }
    case FloorStep::Battle: {
        QString enemyId = resolveEnemyId(step);
        emit battleStarting(enemyId, step.layer);
        break;
    }
    case FloorStep::CardPick: {
        QList<QString> ids = generateCardPickOptions();
        emit cardPickOptionsAvailable(ids);
        break;
    }
    case FloorStep::Rest: {
        int heal = static_cast<int>(m_playerData.maxHp * 0.3);
        emit restOptionAvailable(heal);
        break;
    }
    default: break;
    }
}

// ═══════════════════════════════════
// 选项生成
// ═══════════════════════════════════
QList<QString> RunManager::generateBlessingOptions() {
    QList<QString> all = BlessingDatabase::instance().allIds();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(all.begin(), all.end(), g);
    return all.mid(0, qMin(3, all.size()));
}

QList<QString> RunManager::generateCardPickOptions() {
    QList<QString> pool = {"strike", "defend", "clear_burst",
                           "triple_letter", "digit_diff_strength"};
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(pool.begin(), pool.end(), g);
    return pool.mid(0, qMin(3, pool.size()));
}

// ═══════════════════════════════════
// 回调
// ═══════════════════════════════════
void RunManager::onBlessingChosen(const QString& id) {
    m_playerData.blessings.append(id);
    m_floor++;
    nextFloor();
}

void RunManager::onCardChosen(const QString& id) {
    m_playerData.deck.append(Card{id});
    m_floor++;
    nextFloor();
}

void RunManager::onRestChosen() {
    int heal = static_cast<int>(m_playerData.maxHp * 0.3);
    m_playerData.hp = qMin(m_playerData.maxHp, m_playerData.hp + heal);
    m_floor++;
    nextFloor();
}

void RunManager::updatePlayerData(const QList<Card>& deck, int hp, int strength) {
    m_playerData.deck = deck;
    m_playerData.hp = hp;
    m_playerData.strength = strength;
}

void RunManager::onBattleFinished(bool victory) {
    if (!victory) {
        emit runFinished();
        return;
    }
    m_floor++;
    nextFloor();
}