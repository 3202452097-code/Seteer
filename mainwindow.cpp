#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "runmanager.h"
#include "game.h"
#include "EventPage.h"
#include "GameOverMenu.h"
#include "blessingdata.h"
#include "carddata.h"
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Seteer");
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showMainMenu()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::startGame(bool loadSave)
{
    Q_UNUSED(loadSave);
    cleanupRun();

    m_runManager = new RunManager(this);

    connect(m_runManager, &RunManager::runFinished,
            this, &MainWindow::onRunFinished);
    connect(m_runManager, &RunManager::blessingOptionsAvailable,
            this, &MainWindow::onBlessingOptions);
    connect(m_runManager, &RunManager::cardPickOptionsAvailable,
            this, &MainWindow::onCardPickOptions);
    connect(m_runManager, &RunManager::restOptionAvailable,
            this, &MainWindow::onRestOption);
    connect(m_runManager, &RunManager::battleStarting,
            this, &MainWindow::onBattleFloor);

    connect(this, &MainWindow::blessingSelected,
            m_runManager, &RunManager::onBlessingChosen);
    connect(this, &MainWindow::cardSelected,
            m_runManager, &RunManager::onCardChosen);
    connect(this, &MainWindow::restSelected,
            m_runManager, &RunManager::onRestChosen);
    m_runManager->setDebugEnemy("genius");
    m_runManager->start();
}

void MainWindow::cleanupRun()
{
    if (m_game) {
        ui->stackedWidget->removeWidget(m_game);
        m_game->deleteLater();
        m_game = nullptr;
    }
    if (m_eventPage) {
        ui->stackedWidget->removeWidget(m_eventPage);
        m_eventPage->deleteLater();
        m_eventPage = nullptr;
    }
    if (m_runManager) {
        m_runManager->deleteLater();
        m_runManager = nullptr;
    }
}

// ══════════════════════════════════════════
// ★ 关键：每个事件方法都用 addWidget + setCurrentWidget
// ══════════════════════════════════════════

void MainWindow::onBlessingOptions(const QList<QString>& ids)
{
    qDebug() << "[DEBUG] onBlessingOptions received" << ids.size() << "blessings:" << ids;

    // 清理旧 EventPage
    if (m_eventPage) {
        ui->stackedWidget->removeWidget(m_eventPage);
        m_eventPage->deleteLater();
        m_eventPage = nullptr;
    }

    m_eventPage = new EventPage(this);
    connect(m_eventPage, &EventPage::blessingChosen, this, &MainWindow::blessingSelected);
    connect(m_eventPage, &EventPage::cardChosen,     this, &MainWindow::cardSelected);
    connect(m_eventPage, &EventPage::restChosen,     this, &MainWindow::restSelected);

    QList<QPair<QString, QString>> options;
    for (const QString& id : ids) {
        const BlessingData* b = BlessingDatabase::instance().blessingById(id);
        if (b) options.append({b->name, b->desc});
    }
    m_eventPage->showBlessingOptions(ids, options);

    // ★ 加进 stackedWidget 并切换
    ui->stackedWidget->addWidget(m_eventPage);
    ui->stackedWidget->setCurrentWidget(m_eventPage);
}

void MainWindow::onCardPickOptions(const QList<QString>& ids)
{
    if (m_eventPage) {
        ui->stackedWidget->removeWidget(m_eventPage);
        m_eventPage->deleteLater();
        m_eventPage = nullptr;
    }

    m_eventPage = new EventPage(this);
    connect(m_eventPage, &EventPage::blessingChosen, this, &MainWindow::blessingSelected);
    connect(m_eventPage, &EventPage::cardChosen,     this, &MainWindow::cardSelected);
    connect(m_eventPage, &EventPage::restChosen,     this, &MainWindow::restSelected);

    QList<QPair<QString, QString>> options;
    for (const QString& id : ids) {
        const CardData* cd = CardDatabase::instance().cardById(id);
        if (cd) options.append({cd->name, cd->desc});
    }
    m_eventPage->showCardPickOptions(ids, options);

    ui->stackedWidget->addWidget(m_eventPage);
    ui->stackedWidget->setCurrentWidget(m_eventPage);
}

void MainWindow::onRestOption(int healAmount)
{
    if (m_eventPage) {
        ui->stackedWidget->removeWidget(m_eventPage);
        m_eventPage->deleteLater();
        m_eventPage = nullptr;
    }

    m_eventPage = new EventPage(this);
    connect(m_eventPage, &EventPage::blessingChosen, this, &MainWindow::blessingSelected);
    connect(m_eventPage, &EventPage::cardChosen,     this, &MainWindow::cardSelected);
    connect(m_eventPage, &EventPage::restChosen,     this, &MainWindow::restSelected);

    m_eventPage->showRestOption(healAmount);

    ui->stackedWidget->addWidget(m_eventPage);
    ui->stackedWidget->setCurrentWidget(m_eventPage);
}

void MainWindow::onBattleFloor(const QString& enemyId, int layer)
{
    m_currentLayer = layer;   // ★ 记录当前战斗层数
    Q_UNUSED(layer);
    if (m_game) {
        ui->stackedWidget->removeWidget(m_game);
        m_game->deleteLater();
        m_game = nullptr;
    }
    if (m_eventPage) {
        ui->stackedWidget->removeWidget(m_eventPage);
        m_eventPage->deleteLater();
        m_eventPage = nullptr;
    }

    m_game = new Game(this);
    connect(m_game, &Game::battleFinished, this, &MainWindow::onBattleFinished);

    if (m_runManager) {
        m_game->setRunDeck(m_runManager->playerDeck());
        m_game->setRunBlessings(m_runManager->playerBlessings());
        m_game->setPlayerStartHP(m_runManager->playerHP());
        m_game->setPlayerStartStrength(0);
        m_game->setEnemyId(enemyId);
    }

    ui->stackedWidget->addWidget(m_game);
    ui->stackedWidget->setCurrentWidget(m_game);
    m_game->initGame(1280, 720, "");
}

void MainWindow::startBattleWithData(const QList<Card>& deck,
                                     const QList<QString>& blessings,
                                     int hp, int strength)
{
    if (!m_game) return;
    m_game->setRunDeck(deck);
    m_game->setRunBlessings(blessings);
    m_game->setPlayerStartHP(hp);
    m_game->setPlayerStartStrength(strength);
}
/*原函数 为加入最终胜利图暂时注释
void MainWindow::onBattleFinished(bool victory)
{
    GameOverMenu* menu = new GameOverMenu(victory, this);
    menu->setGeometry((width() - 300) / 2, (height() - 180) / 2, 300, 180);
    menu->show();
    bool* returning = new bool(false);   // ★ 堆上分配，lambda 捕获
    connect(menu, &GameOverMenu::returnToMenu, this, [this, menu, victory, returning]() {
        if (*returning) return;          // ★ 防止重复触发
        *returning = true;
        menu->deleteLater();
        if (m_game && victory) {
            m_runManager->updatePlayerData(
                m_game->runDeck(),
                m_game->player().hp(),
                0);
        }
        if (m_runManager) {
            m_runManager->onBattleFinished(victory);
        }
        delete returning;
    });
}*/
//新函数 加入最终胜利图版本
void MainWindow::onBattleFinished(bool victory)
{
    if (!victory) {
        // 失败：显示失败菜单，点击后继续
        GameOverMenu* menu = new GameOverMenu(false, this);
        menu->setGeometry((width() - 300) / 2, (height() - 180) / 2, 300, 180);
        menu->show();
        connect(menu, &GameOverMenu::returnToMenu, this, [this, menu]() {
            menu->deleteLater();
            if (m_runManager) m_runManager->onBattleFinished(false);
        });
        return;
    }

    // 胜利
    if (m_currentLayer == 4) {
        // ★ 最终胜利：显示胜利画面，延迟继续
        showVictoryScreenAndContinue();
    } else {
        // 普通胜利：直接继续
        if (m_runManager) m_runManager->onBattleFinished(true);
        // 清理游戏界面（可选）
        if (m_game) {
            ui->stackedWidget->removeWidget(m_game);
            m_game->deleteLater();
            m_game = nullptr;
        }
    }
}
//新函数 end
void MainWindow::onRunFinished()
{
    cleanupRun();
    showMainMenu();
}

// ==================== 按钮 ====================
void MainWindow::on_newGameButton_clicked()
{
    startGame(false);
}

void MainWindow::on_loadGameButton_clicked()
{
    // [临时注释] 读档暂不可用
}

void MainWindow::on_exitButton_clicked()
{
    close();
}

void MainWindow::on_actionSaveGame_triggered()
{
    // [临时注释] 存档暂不可用
}
void MainWindow::showVictoryScreenAndContinue()
{
    // 创建一个全屏遮罩 Widget
    QWidget* victoryWidget = new QWidget(this);
    victoryWidget->setGeometry(rect());
    victoryWidget->setStyleSheet("background: rgba(0,0,0,200);");

    QVBoxLayout* layout = new QVBoxLayout(victoryWidget);
    layout->setAlignment(Qt::AlignCenter);

    // 胜利文字
    QLabel* label = new QLabel("胜利！", victoryWidget);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: gold; font-size: 48px; font-weight: bold;");
    layout->addWidget(label);
    QPixmap victoryPix(":/resources/cards/victory.jpg");
    if (!victoryPix.isNull()) {
        QLabel* imageLabel = new QLabel(victoryWidget);
        imageLabel->setPixmap(victoryPix.scaled(300, 300, Qt::KeepAspectRatio));
        imageLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(imageLabel);
    }

    victoryWidget->show();

    // 延迟 2.5 秒后关闭遮罩并继续流程
    QTimer::singleShot(2500, this, [this, victoryWidget]() {
        victoryWidget->deleteLater();
        if (m_runManager) m_runManager->onBattleFinished(true);
        if (m_game) {
            ui->stackedWidget->removeWidget(m_game);
            m_game->deleteLater();
            m_game = nullptr;
        }
    });
}