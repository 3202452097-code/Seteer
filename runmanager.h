#ifndef RUNMANAGER_H
#define RUNMANAGER_H

#include <QObject>
#include <QList>
#include "Card.h"

class RunManager : public QObject
{
    Q_OBJECT
public:
    explicit RunManager(QObject* parent = nullptr);

    void start();

    // ═══════════════════════════════════
    // 楼层结构
    // ═══════════════════════════════════
    struct FloorStep {
        enum Type { Blessing, Battle, CardPick, Rest, End };
        Type type;
        QString enemyId;   // Battle 专用：空 = 从该层随机；非空 = debug 指定
        int layer = 1;     // Battle 专用：第几层
    };

    // ═══════════════════════════════════
    // Debug 接口（MainWindow 调试菜单调用）
    // ═══════════════════════════════════
    void setDebugEnemy(const QString& enemyId);   // 空字符串 = 恢复随机
    void setDebugInitialDeck(const QList<Card>& deck);  // 空列表 = 用默认

    // ═══════════════════════════════════
    // 数据 getter
    // ═══════════════════════════════════
    QList<Card> playerDeck() const     { return m_playerData.deck; }
    QList<QString> playerBlessings() const { return m_playerData.blessings; }
    int playerHP() const               { return m_playerData.hp; }
    int playerStrength() const         { return m_playerData.strength; }

signals:
    void runFinished();
    void blessingOptionsAvailable(const QList<QString>& ids);
    void cardPickOptionsAvailable(const QList<QString>& ids);
    void restOptionAvailable(int healAmount);
    void battleStarting(const QString& enemyId, int layer);   // ★ 带参数

public slots:
    void onBlessingChosen(const QString& id);
    void onCardChosen(const QString& id);
    void onRestChosen();
    void onBattleFinished(bool victory);
    void updatePlayerData(const QList<Card>& deck, int hp, int strength);

private:
    QList<FloorStep> m_floorPlan;
    int m_floor = 0;
    QString m_debugEnemy;          // debug 强制敌人（空 = 随机）
    QList<Card> m_debugDeck;       // debug 初始牌组（空 = 默认）

    void nextFloor();
    void executeFloor(const FloorStep& step);
    QList<QString> generateBlessingOptions();
    QList<QString> generateCardPickOptions();

    // 敌人选择逻辑
    QString resolveEnemyId(const FloorStep& step) const;

    // 默认牌组
    static QList<Card> defaultDeck();

    struct PlayerRunData {
        int hp = 70;
        int maxHp = 70;
        int maxEnergy = 3;
        int strength = 0;
        QList<QString> blessings;
        QList<Card> deck;
    } m_playerData;
};

#endif // RUNMANAGER_H