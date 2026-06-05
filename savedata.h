#ifndef SAVEDATA_H
#define SAVEDATA_H
#include <QString>
#include <QJsonObject>
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
#endif