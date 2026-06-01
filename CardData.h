#ifndef CARDDATA_H
#define CARDDATA_H
#include <QString>
#include <map>    // ← 替换 QMap
#include <vector>
#include <memory>
#include "Effect.h"
struct CardData {
    QString id;
    QString name;
    int cost = 0;
    QString desc;
    QString imagePath;
    std::vector<Effect> effects;
};
class CardDatabase {
public:
    static CardDatabase& instance();
    const CardData* cardById(const QString& id) const;
private:
    CardDatabase();
    void registerDefaults();
    // ★ QMap → unordered_map
    std::map<QString, std::unique_ptr<CardData>> m_cards;
};
#endif // CARDDATA_H