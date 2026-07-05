#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <QString>
#include <QJsonObject>
#include <QList>
#include "Card.h"

struct SaveData
{
    int playerHP = 70;
    int playerBlock = 0;
    int playerStrength = 0;
    int enemyHP = 40;
    QString stringSpace;
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& obj);
};

// ★ 运行存档（用于 RunManager）
struct RunSaveData
{
    int floor = 0;
    int hp = 70;
    int maxHp = 70;
    int strength = 0;
    QList<QString> blessings;
    QList<Card> deck;   // ← 确保是 Card，不是 int

    QJsonObject toJson() const;
    void fromJson(const QJsonObject& obj);
};

#endif // SAVEDATA_H