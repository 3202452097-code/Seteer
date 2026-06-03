#ifndef ENEMYDATA_H
#define ENEMYDATA_H

#include <QString>
#include <QList>
#include <map>
#include <memory>
#include <functional>
#include "entity.h"

class EnemyDatabase {
public:
    static EnemyDatabase& instance();

    std::unique_ptr<Enemy> createEnemy(const QString& id) const;
    QList<QString> enemyIdsForLayer(int layer) const;
    QString randomEnemyForLayer(int layer) const;
    int layerCount() const { return 4; }
    bool hasEnemy(const QString& id) const;

private:
    EnemyDatabase();
    void registerDefaults();

    using Factory = std::function<std::unique_ptr<Enemy>()>;
    std::map<QString, Factory> m_factories;
    std::map<int, QList<QString>> m_layerPools;
};

#endif // ENEMYDATA_H