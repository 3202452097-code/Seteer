#ifndef ENEMYDATA_H
#define ENEMYDATA_H

#include <QString>
#include <QList>
#include <map>
#include <memory>
#include "entity.h"

// 和 ConfigurableAI::Step 共享结构
struct EnemyData {
    QString id;
    QString name;
    int maxHp = 40;
    QList<ConfigurableAI::Step> aiPattern;   // 循环执行的 AI 步骤
    QString spritePath;             // 动画预留
};

class EnemyDatabase {
public:
    static EnemyDatabase& instance();

    const EnemyData* enemyById(const QString& id) const;

    // 按层获取敌人列表
    QList<QString> enemyIdsForLayer(int layer) const;
    // 从某层随机抽一个敌人 ID
    QString randomEnemyForLayer(int layer) const;

    int layerCount() const { return 4; }

private:
    EnemyDatabase();
    void registerDefaults();

    // layer → enemy id 列表
    std::map<int, QList<QString>> m_layerPools;
    // id → 敌人数据
    std::map<QString, std::unique_ptr<EnemyData>> m_enemies;
};

#endif // ENEMYDATA_H