#ifndef ABILITYDATA_H
#define ABILITYDATA_H
#include <QString>
#include <vector>
#include <memory>
#include <map>
#include "effect.h"

// 触发时机
enum class Trigger {
    OnTurnStart,    // 回合开始时
    OnWriteString,  // 写入字符串空间后
    OnCardPlayed,    // 打出一张牌后
    OnTurnEnd,       //回合结束时
};

struct AbilityData {
    QString id;
    QString name;
    QString desc;
    Trigger trigger;        // 触发时机
    std::unique_ptr<EffectAction> action;   // 直接存储动作，无条件
    std::unique_ptr<Condition> condition;   // ★ 新增条件
    int defaultDuration = 0; // 0=永久, >0=回合数
};

class AbilityDatabase {
public:
    static AbilityDatabase& instance();
    const AbilityData* abilityById(const QString& id) const;
private:
    AbilityDatabase();
    void registerDefaults();
    std::map<QString, std::unique_ptr<AbilityData>> m_abilities;
};
#endif