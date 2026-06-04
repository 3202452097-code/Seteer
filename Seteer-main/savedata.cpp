#include "SaveData.h"
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
