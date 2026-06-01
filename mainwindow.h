
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

    class Game;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showMainMenu();  // 显示主菜单（显示自己）
    void hideMainMenu();  // 隐藏主菜单（进入游戏）

private slots:
    void on_newGameButton_clicked();
    void on_loadGameButton_clicked();
    void on_exitButton_clicked();
    void on_actionSaveGame_triggered();

private:
    Ui::MainWindow *ui;

    Game* m_game = nullptr;

    void startGame(bool loadSave = false);
};

#endif // MAINWINDOW_H