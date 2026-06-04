#ifndef BATTLECONTEXT_H
#define BATTLECONTEXT_H

#include <QList>
#include <algorithm>
#include <random>
#include "Entity.h"
#include "StringSpace.h"
#include "Card.h"
struct BattleContext {
    int turnCount = 0;
    // 实体
    Player* player = nullptr;
    Enemy* enemy = nullptr;
    // 字符串空间
    StringSpace* stringSpace = nullptr;
    // 牌堆
    QList<Card>* drawPile = nullptr;
    QList<Card>* hand = nullptr;
    QList<Card>* discardPile = nullptr;
    // 当前攻击者
    Entity* attacker = nullptr;
    Entity* defender = nullptr;

    std::function<void(int)> drawCards;
};
#endif // BATTLECONTEXT_H
