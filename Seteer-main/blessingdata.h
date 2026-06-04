#ifndef BLESSINGDATA_H
#define BLESSINGDATA_H
#include <QString>
#include <QList>
#include <map>
#include <vector>
#include <memory>
#include "Effect.h"
struct BlessingData {
    QString id;
    QString name;
    QString desc;
    std::vector<Effect> effects;   // ★ 复用 Effect 体系！
};
class BlessingDatabase {
public:
    static BlessingDatabase& instance();
    const BlessingData* blessingById(const QString& id) const;
    QList<QString> allIds() const;
private:
    BlessingDatabase();
    void registerDefaults();
    std::map<QString, std::unique_ptr<BlessingData>> m_blessings;
};
#endif