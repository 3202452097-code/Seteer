#include "game.h"
#include "carditem.h"
#include "SaveData.h"
#include "GameOverMenu.h"
#include "mainwindow.h"
#include <QMouseEvent>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QPixmap>
// ==================== 构造 ====================
Game::Game(QWidget *parent)
    : QGraphicsView{parent},
    m_player(70, 3),
    m_enemy(40, std::make_unique<SimpleAI>())
{
    m_scene = new QGraphicsScene(this);

    setScene(m_scene);

    setRenderHint(QPainter::Antialiasing,true);
    setRenderHint(QPainter::SmoothPixmapTransform,true);
    setRenderHint(QPainter::TextAntialiasing, true);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 视口更新模式 - 关键设置！
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // 缓存背景
    setCacheMode(QGraphicsView::CacheBackground);

    // 优化标志
    setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);

    // 结束回合按钮
    m_endTurnButton = m_scene->addRect(0, 0, 120, 40,
                                       QPen(Qt::white, 2),
                                       QBrush(QColor(80, 30, 30)));
    m_endTurnLabel = m_scene->addText("结束回合");
    m_endTurnLabel->setDefaultTextColor(Qt::white);
    m_endTurnLabel->setFont(QFont("Sans", 14));
    // 信息文字
    m_playerInfoText = m_scene->addText("");
    m_playerInfoText->setDefaultTextColor(Qt::white);
    m_playerInfoText->setFont(QFont("Sans", 12));
    m_enemyInfoText = m_scene->addText("");
    m_enemyInfoText->setDefaultTextColor(Qt::red);
    m_enemyInfoText->setFont(QFont("Sans", 12));
    m_pileInfoText = m_scene->addText("");
    m_pileInfoText->setDefaultTextColor(QColor(180, 180, 180));
    m_pileInfoText->setFont(QFont("Sans", 10));
    m_stringSpaceText = m_scene->addText("");
    m_stringSpaceText->setDefaultTextColor(QColor(255, 220, 100));
    m_stringSpaceText->setFont(QFont("Sans", 12));
    // 字符串空间变化时自动刷新 UI
    m_stringSpace.onChanged = [this]() { updateUI(); };
}



// ==================== 初始化 ====================
void Game::initGame(int w, int h, const QString& title, const QIcon& icon) {
    setFixedSize(w, h);
    setWindowTitle(title);
    if (!icon.isNull()) {
        setWindowIcon(icon);
    }
    m_scene->setSceneRect(0, 0, w, h);

    QPixmap bgPixmap(":/resources/cards/bg.jpg");  // 使用主界面同样的背景图
    if (!bgPixmap.isNull()) {
        // 缩放背景图以适应窗口大小
        QPixmap scaledBg = bgPixmap.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        m_scene->setBackgroundBrush(QBrush(scaledBg));
    } else {
        // 如果图片加载失败，设置一个深色渐变背景作为备选
        QLinearGradient gradient(0, 0, 0, h);
        gradient.setColorAt(0, QColor(30, 30, 50));
        gradient.setColorAt(1, QColor(20, 20, 40));
        m_scene->setBackgroundBrush(QBrush(gradient));
    }

    initBattle();
}
void Game::run() {
    this->show();
}
// ==================== BattleContext 工厂 ====================
BattleContext Game::buildContext() const {
    BattleContext ctx;
    ctx.player      = const_cast<Player*>(&m_player);
    ctx.enemy       = const_cast<Enemy*>(&m_enemy);
    ctx.stringSpace = const_cast<StringSpace*>(&m_stringSpace);
    ctx.drawPile    = const_cast<QList<Card>*>(&m_drawPile);
    ctx.hand        = const_cast<QList<Card>*>(&m_hand);
    ctx.discardPile = const_cast<QList<Card>*>(&m_discardPile);
    return ctx;
}
// ==================== 战斗初始化 ====================
void Game::initBattle() {
    m_drawPile.clear();
    m_hand.clear();
    m_discardPile.clear();
    for (auto* item : m_handItems) m_scene->removeItem(item);
    m_handItems.clear();
    m_selectedCard = nullptr;
    // 卡组
    for (int i = 0; i < 5; i++) m_drawPile.append(Card{ "strike" });
    for (int i = 0; i < 5; i++) m_drawPile.append(Card{ "defend" });
    for (int i = 0; i < 2; i++) m_drawPile.append(Card{ "random_letter" });
    for (int i = 0; i < 2; i++) m_drawPile.append(Card{ "random_digit" });
    m_drawPile.append(Card{ "clear_burst" });
    m_drawPile.append(Card{ "triple_letter" });
    m_drawPile.append(Card{ "digit_diff_strength" });

    // 洗牌
    BattleContext ctx = buildContext();
    ctx.shuffleDiscardIntoDraw(); // 先把 drawPile 洗了
    // 上面的调用等价于手动洗 drawPile，不过 drawPile 目前就是 10 张，discardPile 是空的
    // 实际上这里直接洗 drawPile：
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_drawPile.begin(), m_drawPile.end(), g);
    // 重置实体
    m_player.setHP(70);
    m_player.setBlock(0);
    m_player.setStrength(2);
    m_player.restoreEnergy();
    m_enemy.setHP(40);
    m_enemy.setBlock(0);
    m_enemy.setStrength(0);
    m_enemy.setAI(std::make_unique<PatternAI>());
    // 清空字符串空间
    m_stringSpace.clear();
    m_state = PlayerTurn;
    m_enemy.decideIntent(ctx);   // ← 第一回合就决定意图
    startPlayerTurn();
    updateUI();
}
// ==================== 回合 ====================
void Game::startPlayerTurn() {
    BattleContext ctx = buildContext();
    m_player.onTurnStart(ctx);   // 回能 + 清格挡
    drawCards(5);
    m_state = PlayerTurn;
    qInfo() << "--- 玩家回合开始 ---";
    updateUI();
}
void Game::endPlayerTurn() {
    qInfo() << "--- 玩家回合结束 ---";
    discardHand();
    m_state = EnemyTurn;
    enemyAction();
    updateUI();
}
// ==================== 抽牌 ====================
void Game::drawCards(int count) {
    BattleContext ctx = buildContext();
    for (int i = 0; i < count; i++) {
        if (m_drawPile.isEmpty()) {
            ctx.shuffleDiscardIntoDraw();
            if (m_drawPile.isEmpty()) break;
        }
        Card c = m_drawPile.takeLast();
        m_hand.append(c);
        auto* item = new CardItem(c, m_hand.size() - 1, this);
        m_scene->addItem(item);
        m_handItems.append(item);
    }
    rearrangeHand();
}
// ==================== 出牌 ====================
void Game::playCard(int handIndex) {
    if (handIndex < 0 || handIndex >= m_hand.size()) return;
    Card card = m_hand[handIndex];
    const CardData* data = card.data();
    if (!data) return;
    // 检查费用
    if (data->cost > m_player.energy()) {
        qInfo() << "能量不足！需要" << data->cost << "，当前" << m_player.energy();
        rearrangeHand();
        return;
    }
    // 扣能量
    m_player.setEnergy(m_player.energy() - data->cost);
    // ── 构建战场上下文 ──
    BattleContext ctx = buildContext();
    ctx.attacker = &m_player;
    // ── 依次执行所有 Effect ──
    for (auto& effect : data->effects) {
        effect.execute(ctx);
    }
    // 移除 UI
    CardItem* item = m_handItems.takeAt(handIndex);
    m_scene->removeItem(item);
    delete item;
    // 手牌 → 弃牌堆
    m_hand.removeAt(handIndex);
    m_discardPile.append(card);
    m_selectedCard = nullptr;
    for (int i = 0; i < m_handItems.size(); i++) {
        m_handItems[i]->setHandIndex(i);
    }
    rearrangeHand();
    updateUI();
    if (m_enemy.isDead()) {
        m_state = GameOver;
        qInfo() << "战斗胜利！";
        showGameOverMenu();
    }
}
// ==================== 弃牌 ====================
void Game::discardHand() {
    for (Card c : m_hand) {
        m_discardPile.append(c);
    }
    m_hand.clear();
    for (auto* item : m_handItems) {
        m_scene->removeItem(item);
        delete item;
    }
    m_handItems.clear();
    m_selectedCard = nullptr;
}
// ==================== 敌人 ====================
void Game::enemyAction() {
    if (m_state == GameOver) return;
    qInfo() << "--- 敌人回合 ---";
    BattleContext ctx = buildContext();
    // 执行敌人回合开始（清格挡 + 执行自身 buff）
    m_enemy.onTurnStart(ctx);
    // 攻击
    int dmg = m_enemy.intentDamage() + m_enemy.strength();  // ★ 力量加成
    if (dmg > 0) {
        m_player.takeDamage(dmg);
        qInfo() << "敌人攻击，造成" << dmg << "点伤害";
    }
    checkGameOver();
    if (m_state != GameOver) {
        // ★ 决定下一回合意图（让玩家在自己的回合看到）
        m_enemy.decideIntent(ctx);
        startPlayerTurn();
    }
}
void Game::checkGameOver() {
    if (m_player.isDead()) {
        m_state = GameOver;
        qInfo() << "玩家死亡...";
        showGameOverMenu();
    } else if (m_enemy.isDead()) {
        m_state = GameOver;
        qInfo() << "战斗胜利！";
        showGameOverMenu();
    }
}

// ==================== 新增：返回主菜单 ====================
void Game::returnToMainMenu() {
    // 关闭游戏结束菜单
    if (m_gameOverMenu) {
        m_gameOverMenu->close();
        delete m_gameOverMenu;
        m_gameOverMenu = nullptr;
    }

    // 关闭游戏窗口
    close();

    // 注意：game 窗口关闭后，由于设置了 WA_DeleteOnClose，
    // 会自动删除，并触发 destroyed 信号，让 MainWindow 重新显示
}

// ==================== 新增：显示游戏结束菜单 ====================
void Game::showGameOverMenu() {
    bool isVictory = m_enemy.isDead();

    if (m_gameOverMenu) {
        delete m_gameOverMenu;
        m_gameOverMenu = nullptr;
    }

    m_gameOverMenu = new GameOverMenu(this, isVictory, nullptr);

    // 居中显示
    QPoint center = mapToGlobal(geometry().center());
    m_gameOverMenu->move(center.x() - m_gameOverMenu->width() / 2,
                         center.y() - m_gameOverMenu->height() / 2);
    m_gameOverMenu->show();
}


// ==================== CardItem 回调 ====================
void Game::onCardClicked(CardItem* item) {
    if (m_selectedCard == item) {
        m_selectedCard = nullptr;
    } else {
        if (m_selectedCard) rearrangeHand();
        m_selectedCard = item;
        QPointF p = item->scenePos();
        item->setPos(p.x(), p.y() - 20);
    }
    updateUI();
}
void Game::onCardDragged(CardItem* item, const QPointF& releasePos) {
    if (releasePos.y() < PLAY_ZONE_Y) {
        playCard(item->handIndex());
    } else {
        rearrangeHand();
        m_selectedCard = nullptr;
    }
    updateUI();
}
// ==================== 手牌排列 ====================
void Game::rearrangeHand() {
    int n = m_handItems.size();
    if (n == 0) return;
    int totalWidth = (n - 1) * CARD_OVERLAP + CARD_WIDTH;
    int startX = (width() - totalWidth) / 2;
    int y = height() - HAND_Y_OFFSET;
    for (int i = 0; i < n; i++) {
        CardItem* item = m_handItems[i];
        if (item == m_selectedCard) continue;
        item->snapTo(QPointF(startX + i * CARD_OVERLAP, y));
        item->setZValue(i);
    }
}
// ==================== UI 更新 ====================
void Game::updateUI() {
    // 玩家信息
    QString playerText = QString("玩家 HP: %1/%2 | 格挡: %3 | 力量: %4 | 能量: %5/%6")
                             .arg(m_player.hp())
                             .arg(m_player.maxHp())
                             .arg(m_player.block())
                             .arg(m_player.strength())          // ★
                             .arg(m_player.energy())
                             .arg(m_player.maxEnergy());
    m_playerInfoText->setPlainText(playerText);
    m_playerInfoText->setPos(20, 20);
    // 敌人信息
    QString enemyText = QString("敌人 HP: %1/%2 | 格挡: %3 | 力量: %4 | 意图: %5")
                            .arg(m_enemy.hp())
                            .arg(m_enemy.maxHp())
                            .arg(m_enemy.block())
                            .arg(m_enemy.strength())              // ★
                            .arg(m_enemy.intentDescription());
    m_enemyInfoText->setPlainText(enemyText);
    m_enemyInfoText->setPos(20, 50);
    // 牌堆信息
    QString pileText = QString("抽牌堆: %1 | 弃牌堆: %2 | 手牌: %3")
                           .arg(m_drawPile.size())
                           .arg(m_discardPile.size())
                           .arg(m_hand.size());
    m_pileInfoText->setPlainText(pileText);
    m_pileInfoText->setPos(width() - 250, 20);
    // 字符串空间
    QString ssText = QString("字符串空间: \"%1\" (长度 %2)")
                         .arg(m_stringSpace.content())
                         .arg(m_stringSpace.length());
    m_stringSpaceText->setPlainText(ssText);
    m_stringSpaceText->setPos(20, 80);
    // 结束回合按钮
    m_endTurnButton->setPos(width() - 160, height() - 60);
    m_endTurnLabel->setPos(width() - 150, height() - 55);
    if (m_state == GameOver) {
        m_endTurnLabel->setPlainText(m_player.isDead() ? "失败" : "胜利");
        m_endTurnButton->setBrush(QColor(60, 60, 60));
    }
}
// ==================== 鼠标点击 ====================
void Game::mousePressEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->pos());
    if (m_endTurnButton->contains(m_endTurnButton->mapFromScene(scenePos))
        && m_state == PlayerTurn) {
        endPlayerTurn();
        return;
    }
    QGraphicsItem* clickedItem = m_scene->itemAt(scenePos, transform());
    if (!clickedItem || clickedItem == m_endTurnButton || clickedItem == m_endTurnLabel) {
        if (m_selectedCard) {
            m_selectedCard = nullptr;
            rearrangeHand();
            updateUI();
        }
    }
    QGraphicsView::mousePressEvent(event);
}


// ==================== 保存/加载 ====================
void Game::saveGame()
{
    SaveData data;

    data.playerHP = m_player.hp();

    data.playerBlock = m_player.block();

    data.playerStrength = m_player.strength();

    data.enemyHP = m_enemy.hp();

    data.stringSpace = m_stringSpace.content();

    QFile file("save.json");

    if (file.open(QIODevice::WriteOnly))
    {
        QJsonDocument doc(data.toJson());

        file.write(doc.toJson());

        file.close();

        qInfo() << "游戏已保存";
    }
}

void Game::loadGame()
{
    QFile file("save.json");

    if (!file.open(QIODevice::ReadOnly))
    {
        qInfo() << "没有找到存档";

        return;
    }

    QByteArray bytes = file.readAll();

    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(bytes);

    SaveData data;

    data.fromJson(doc.object());

    m_player.setHP(data.playerHP);

    m_player.setBlock(data.playerBlock);

    m_player.setStrength(data.playerStrength);

    m_enemy.setHP(data.enemyHP);

    m_stringSpace.setText(data.stringSpace);

    updateUI();

    qInfo() << "读档成功";
}