#ifndef RUNMANAGER_H
#define RUNMANAGER_H
#include <QObject>
#include <QList>
#include "Card.h"
class Game;
class RunManager : public QObject
{
    Q_OBJECT
public:
    explicit RunManager(QObject* parent = nullptr);
    void start();
    // ★ 楼层类型（供 MainWindow 使用）
    enum FloorType { Blessing, Battle, CardPick, Rest, End };
    Game* currentGame() const { return m_currentGame; }
    // 数据 getter（MainWindow 用来传递给 Game）
    QList<Card> playerDeck() const { return m_playerData.deck; }
    QList<QString> playerBlessings() const { return m_playerData.blessings; }
    int playerHP() const { return m_playerData.hp; }
    int playerStrength() const { return m_playerData.strength; }
signals:
    void runFinished();
    // ★ 通知 MainWindow 显示对应事件页
    void blessingOptionsAvailable(const QList<QString>& ids);
    void cardPickOptionsAvailable(const QList<QString>& ids);
    void restOptionAvailable(int healAmount);
    void battleStarting();
public slots:
    // ★ MainWindow 回调
    void onBlessingChosen(const QString& id);
    void onCardChosen(const QString& id);
    void onRestChosen();
    void onBattleFinished(bool victory);
    void updatePlayerData(const QList<Card>& deck, int hp, int strength);
private:
    int m_floor = 0;
    QList<FloorType> m_floorPlan;
    QWidget* m_currentEventWidget = nullptr;  // 仅用于生命周期管理
    void nextFloor();
    void executeFloor(FloorType type);
    // 生成选项数据（不创建 UI）
    QList<QString> generateBlessingOptions();
    QList<QString> generateCardPickOptions();
    // 持久数据
    struct PlayerRunData {
        int hp = 70;
        int maxHp = 70;
        int maxEnergy = 3;
        int strength = 0;
        QList<QString> blessings;
        QList<Card> deck;
    } m_playerData;
    Game* m_currentGame = nullptr;
};
#endif // RUNMANAGER_H