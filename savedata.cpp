#include "savedata.h"
#include <QJsonArray>
#include <QFile>

QJsonObject SaveData::toJson() const
{
    QJsonObject obj;
    obj["playerHP"] = playerHP;
    obj["playerBlock"] = playerBlock;
    obj["playerStrength"] = playerStrength;
    obj["enemyHP"] = enemyHP;
    obj["stringSpace"] = stringSpace;
    return obj;
}

void SaveData::fromJson(const QJsonObject& obj)
{
    playerHP = obj["playerHP"].toInt();
    playerBlock = obj["playerBlock"].toInt();
    playerStrength = obj["playerStrength"].toInt();
    enemyHP = obj["enemyHP"].toInt();
    stringSpace = obj["stringSpace"].toString();
}

// ★ RunSaveData 实现
QJsonObject RunSaveData::toJson() const
{
    QJsonObject obj;
    obj["floor"] = floor;
    obj["hp"] = hp;
    obj["maxHp"] = maxHp;
    obj["strength"] = strength;

    QJsonArray blessArr;
    for (const QString& id : blessings)
        blessArr.append(id);
    obj["blessings"] = blessArr;

    QJsonArray deckArr;
    for (const Card& c : deck)
        deckArr.append(c.id);
    obj["deck"] = deckArr;

    return obj;
}

void RunSaveData::fromJson(const QJsonObject& obj)
{
    floor = obj["floor"].toInt();
    hp = obj["hp"].toInt();
    maxHp = obj["maxHp"].toInt();
    strength = obj["strength"].toInt();

    blessings.clear();
    QJsonArray blessArr = obj["blessings"].toArray();
    for (const QJsonValue& v : blessArr)
        blessings.append(v.toString());

    deck.clear();
    QJsonArray deckArr = obj["deck"].toArray();
    for (const QJsonValue& v : deckArr) {
        Card c;
        c.id = v.toString();
        deck.append(c);
    }
}