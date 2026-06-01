#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "game.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Seteer");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showMainMenu()
{
    // 显示主窗口
    show();
    // 激活主窗口
    raise();
    activateWindow();
}

void MainWindow::hideMainMenu()
{
    // 隐藏主窗口
    hide();
}

void MainWindow::startGame(bool loadSave)
{
    // 如果已有游戏实例，先删除
    if (m_game) {
        m_game->deleteLater();
        m_game = nullptr;
    }

    // 创建游戏窗口（作为独立窗口，不嵌入主窗口）
    m_game = new Game();
    m_game->setAttribute(Qt::WA_DeleteOnClose);  // 关闭时自动删除
    m_game->initGame(1280, 720, "Seteer - 游戏中");

    // 连接游戏窗口的销毁信号，以便在游戏关闭时重新显示主菜单
    connect(m_game, &Game::destroyed, this, [this]() {
        showMainMenu();
        m_game = nullptr;
    });

    // 隐藏主菜单，显示游戏
    hideMainMenu();
    m_game->show();

    // 如果需要加载存档
    if (loadSave) {
        m_game->loadGame();
    }
}

void MainWindow::on_newGameButton_clicked()
{
    startGame(false);
}
void MainWindow::on_loadGameButton_clicked()
{
    startGame(true);
}
void MainWindow::on_exitButton_clicked()
{
    close();
}
    void MainWindow::on_actionSaveGame_triggered()
{
    if (m_game&& m_game->isVisible())
    {
        m_game->saveGame();
    }
}
