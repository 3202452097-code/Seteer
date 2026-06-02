#include "runmanager.h"
#include "game.h"
#include "blessingdata.h"
#include "carddata.h"
#include <QRandomGenerator>
#include <algorithm>

RunManager::RunManager(QObject* parent) : QObject(parent) {}

void RunManager::start() {
    m_playerData = PlayerRunData{};
    m_floor = 0;

    m_floorPlan.clear();
    m_floorPlan.append(Blessing);
    for (int i = 0; i < 3; i++) {
        m_floorPlan.append(Battle);
        m_floorPlan.append(CardPick);
        m_floorPlan.append(Rest);
    }
    m_floorPlan.append(End);

    nextFloor();
}

void RunManager::nextFloor() {
    if (m_floor >= m_floorPlan.size() || m_floorPlan[m_floor] == End) {
        emit runFinished();
        return;
    }
    executeFloor(m_floorPlan[m_floor]);
}

void RunManager::executeFloor(FloorType type) {
    switch (type) {
    case Blessing: {
        QList<QString> ids = generateBlessingOptions();
        emit blessingOptionsAvailable(ids);
        break;
    }
    case Battle:
        emit battleStarting();  // MainWindow 创建 Game
        break;
    case CardPick: {
        QList<QString> ids = generateCardPickOptions();
        emit cardPickOptionsAvailable(ids);
        break;
    }
    case Rest: {
        int heal = static_cast<int>(m_playerData.maxHp * 0.3);
        emit restOptionAvailable(heal);
        break;
    }
    default: break;
    }
}


// ==================== 选项生成（只生成数据，不弹窗） ====================
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

// ==================== MainWindow 回调 ====================
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
    m_currentGame = nullptr;

    if (!victory) {
        emit runFinished();
        return;
    }

    // [注意] 需要从 Game 获取最新数据——MainWindow 在 startBattleWithData 中传入，
    // Game 在 initBattle 中消耗 m_runDeck。战斗结束后数据需要通过 runDeck() 取回。
    // 目前 MainWindow 持有 Game 引用，在 onBattleFinished 中调 runDeck() 获取。
    m_floor++;
    nextFloor();
}