#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "Card.h"
class QStackedWidget;
class RunManager;
class Game;
class EventPage;
class GameOverMenu;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    // ★ 转发 RunManager 数据给 Game
    void startBattleWithData(const QList<Card>& deck,
                             const QList<QString>& blessings,
                             int hp, int strength);
signals:
    // ★ 事件页用户选择 → RunManager
    void blessingSelected(const QString& id);
    void cardSelected(const QString& id);
    void restSelected();
private slots:
    void on_newGameButton_clicked();
    void on_loadGameButton_clicked();
    void on_exitButton_clicked();
    void on_actionSaveGame_triggered();
    // 响应 RunManager
    void onRunFinished();
    void onBlessingOptions(const QList<QString>& ids);
    void onCardPickOptions(const QList<QString>& ids);
    void onRestOption(int healAmount);
    void onBattleFloor(const QString& enemyId, int layer);
    // 响应 Game
    void onBattleFinished(bool victory);
private:
    Ui::MainWindow *ui;
    RunManager* m_runManager = nullptr;
    Game* m_game = nullptr;
    EventPage* m_eventPage = nullptr;
    void startGame(bool loadSave = false);
    void cleanupRun();
    void showMainMenu();
};
#endif // MAINWINDOW_H