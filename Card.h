#ifndef CARD_H
#define CARD_H
#include <QString>
#include "carddata.h"
// 卡牌实例（轻量引用，只有 id）
// 需要数据时通过 data() 从数据库查询
struct Card {
    QString id;  // 对应 CardData::id
    const CardData* data() const {
        return CardDatabase::instance().cardById(id);
    }
};
#endif // CARD_H