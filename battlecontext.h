#ifndef BATTLECONTEXT_H
#define BATTLECONTEXT_H
#include <QList>
#include <algorithm>
#include <random>
#include "Entity.h"
#include "StringSpace.h"
#include "Card.h"
struct BattleContext {
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
    // 弃牌堆洗入抽牌堆（DrawAction 会调用）
    // void shuffleDiscardIntoDraw() {
    //     while (!discardPile->isEmpty())
    //         drawPile->append(discardPile->takeLast());
    //     std::random_device rd;
    //     std::mt19937 g(rd());
    //     std::shuffle(drawPile->begin(), drawPile->end(), g);
    // }

    std::function<void(int)> drawCards;
};
#endif // BATTLECONTEXT_H
