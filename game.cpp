#include "game.h"
#include "carditem.h"
#include "blessingdata.h"
#include "enemydata.h"
// [临时注释] 存档系统暂不可用
// #include "SaveData.h"
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
// [临时注释]
// #include <QFile>
// #include <QJsonDocument>
#include <QPixmap>

// ==================== 构造 ====================
Game::Game(QWidget *parent)
    : QGraphicsView{parent},
    m_player(70, 3),
    m_enemy(40, std::make_unique<SimpleAI>())
{
    m_scene = new QGraphicsScene(this);
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setRenderHint(QPainter::TextAntialiasing, true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setCacheMode(QGraphicsView::CacheBackground);
    setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);
    // 结束回合按钮
    m_endTurnButton = m_scene->addRect(0, 0, 150, 40,
                                       QPen(Qt::white, 2),
                                       QBrush(QColor(80, 30, 30)));
    m_endTurnLabel = m_scene->addText("结束第1回合");
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
    m_stringSpace.onChanged = [this]() { updateUI(); };
}



// ==================== 初始化 ====================
void Game::initGame(int w, int h, const QString& title, const QIcon& icon) {
    Q_UNUSED(title);
    if (!icon.isNull()) setWindowIcon(icon);
    m_scene->setSceneRect(0, 0, w, h);
    QPixmap bgPixmap(":/resources/cards/bg.jpg");
    if (!bgPixmap.isNull()) {
        QPixmap scaledBg = bgPixmap.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        m_scene->setBackgroundBrush(QBrush(scaledBg));
    } else {
        QLinearGradient gradient(0, 0, 0, h);
        gradient.setColorAt(0, QColor(30, 30, 50));
        gradient.setColorAt(1, QColor(20, 20, 40));
        m_scene->setBackgroundBrush(QBrush(gradient));
    }
    initBattle();
    viewport()->update();
    update();
}
// ==================== BattleContext 工厂 ====================
BattleContext Game::buildContext() const {
    BattleContext ctx;
    ctx.turnCount = m_turnCount;
    ctx.player      = const_cast<Player*>(&m_player);
    ctx.enemy       = const_cast<Enemy*>(&m_enemy);
    ctx.stringSpace = const_cast<StringSpace*>(&m_stringSpace);
    ctx.drawPile    = const_cast<QList<Card>*>(&m_drawPile);
    ctx.hand        = const_cast<QList<Card>*>(&m_hand);
    ctx.discardPile = const_cast<QList<Card>*>(&m_discardPile);
    ctx.drawCards = [this](int count) {
        const_cast<Game*>(this)->drawCards(count);
    };
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

    // ── 卡组：外部传入 > m_runDeck > 空 ──
    if (!m_initialDeck.isEmpty()) {
        m_drawPile = m_initialDeck;
        m_initialDeck.clear();
    } else if (!m_runDeck.isEmpty()) {
        m_drawPile = m_runDeck;
        m_runDeck.clear();
    }
    // 如果 m_drawPile 仍为空（应该不会），让 drawCards 自己去处理

    // 洗牌
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_drawPile.begin(), m_drawPile.end(), g);

    // ── 重置实体 ──
    m_player.setHP(m_startHP);
    m_player.setBlock(0);
    m_player.setStrength(m_startStrength);
    m_player.restoreEnergy();

    // ── ★ 创建敌人：从 EnemyDatabase ──
    const EnemyData* ed = EnemyDatabase::instance().enemyById(m_enemyId);
    if (ed) {
        QList<ConfigurableAI::Step> steps;
        for (const auto& s : ed->aiPattern) {
            steps.append({s.damage, s.selfBlock, s.strengthGain, s.description});
        }
        m_enemy = Enemy(ed->maxHp, std::make_unique<ConfigurableAI>(steps));
    } else {
        // 兜底：未知敌人 → SimpleAI
        m_enemy = Enemy(40, std::make_unique<SimpleAI>());
    }
    m_enemy.setBlock(0);
    m_enemy.setStrength(0);

    m_stringSpace.clear();

    m_state = PlayerTurn;
    BattleContext ctx = buildContext();
    m_enemy.decideIntent(ctx);
    startPlayerTurn();
    updateUI();
}


// ==================== 回合 ====================
void Game::startPlayerTurn() {
    BattleContext ctx = buildContext();
    m_player.onTurnStart(ctx);
    m_turnCount++;
    ctx.turnCount = m_turnCount;
    ctx.attacker = &m_player;
    // 执行祈福效果
    for (const QString& id : m_runBlessings) {
        const BlessingData* b = BlessingDatabase::instance().blessingById(id);
        if (b) {
            for (auto& effect : b->effects) {
                effect.execute(ctx);
            }
        }
    }
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
    for (int i = 0; i < count; i++) {
        if (m_drawPile.isEmpty()) {
            while (!m_discardPile.isEmpty())
                m_drawPile.append(m_discardPile.takeLast());
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(m_drawPile.begin(), m_drawPile.end(), g);
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
    if (data->cost > m_player.energy()) {
        qInfo() << "能量不足！需要" << data->cost << "，当前" << m_player.energy();
        rearrangeHand();
        return;
    }
    m_player.setEnergy(m_player.energy() - data->cost);
    BattleContext ctx = buildContext();
    ctx.attacker = &m_player;
    for (auto& effect : data->effects) {
        effect.execute(ctx);
        if (m_enemy.isDead()) break;
    }
    CardItem* item = m_handItems.takeAt(handIndex);
    QTimer::singleShot(0, this, [this, item]() {
        m_scene->removeItem(item);
        delete item;
    });
    m_hand.removeAt(handIndex);
    m_discardPile.append(card);
    m_selectedCard = nullptr;
    for (int i = 0; i < m_handItems.size(); i++) {
        m_handItems[i]->setHandIndex(i);
    }
    rearrangeHand();
    updateUI();
    if (m_state != GameOver && m_enemy.isDead()) {
        m_state = GameOver;
        qInfo() << "战斗胜利！";
        emit battleFinished(true);
    }
}
// ==================== 弃牌 ====================
void Game::discardHand() {
    for (Card c : m_hand) m_discardPile.append(c);
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
    m_enemy.onTurnStart(ctx);
    int dmg = (m_enemy.intentDamage() != 0)
                  ? m_enemy.intentDamage() + m_enemy.strength()
                  : 0;
    if (dmg > 0) {
        m_player.takeDamage(dmg);
        qInfo() << "敌人攻击，造成" << dmg << "点伤害";
    }
    checkGameOver();
    if (m_state != GameOver) {
        m_enemy.decideIntent(ctx);
        startPlayerTurn();
    }
}
void Game::checkGameOver() {
    if (m_player.isDead()) {
        m_state = GameOver;
        qInfo() << "玩家死亡...";
        emit battleFinished(false);
    } else if (m_enemy.isDead()) {
        m_state = GameOver;
        qInfo() << "战斗胜利！";
        emit battleFinished(true);
    }
}
// ==================== CardItem 回调 ====================
void Game::onCardClicked(CardItem* item) {
    if (m_state == GameOver) return;
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
    if (m_state == GameOver) return;
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
    QString playerText = QString("玩家 HP: %1/%2 | 格挡: %3 | 力量: %4 | 能量: %5/%6")
                             .arg(m_player.hp()).arg(m_player.maxHp())
                             .arg(m_player.block()).arg(m_player.strength())
                             .arg(m_player.energy()).arg(m_player.maxEnergy());
    m_playerInfoText->setPlainText(playerText);
    m_playerInfoText->setPos(20, 20);
    QString enemyText = QString("敌人 HP: %1/%2 | 格挡: %3 | 力量: %4 | 意图: %5")
                            .arg(m_enemy.hp()).arg(m_enemy.maxHp())
                            .arg(m_enemy.block()).arg(m_enemy.strength())
                            .arg(m_enemy.intentDescription());
    m_enemyInfoText->setPlainText(enemyText);
    m_enemyInfoText->setPos(20, 50);
    QString pileText = QString("抽牌堆: %1 | 弃牌堆: %2 | 手牌: %3")
                           .arg(m_drawPile.size()).arg(m_discardPile.size()).arg(m_hand.size());
    m_pileInfoText->setPlainText(pileText);
    m_pileInfoText->setPos(width() - 250, 20);
    QString ssText = QString("字符串空间: \"%1\" (长度 %2)")
                         .arg(m_stringSpace.content()).arg(m_stringSpace.length());
    m_stringSpaceText->setPlainText(ssText);
    m_stringSpaceText->setPos(20, 80);
    m_endTurnButton->setPos(width() - 180, height() - 120);
    m_endTurnLabel->setPos(width() - 170, height() - 115);
    m_endTurnLabel->setPlainText(QString("结束第 %1 回合").arg(m_turnCount));
    if (m_state == GameOver) {
        m_endTurnLabel->setPlainText(m_player.isDead() ? "失败" : "胜利");
        m_endTurnButton->setBrush(QColor(60, 60, 60));
    }
}
//隐藏结束按钮（仍未调用）
void Game::setEndTurnButtonVisible(bool visible) {
    m_endTurnButton->setVisible(visible);
    m_endTurnLabel->setVisible(visible);
}
// ==================== 鼠标点击 ====================
void Game::mousePressEvent(QMouseEvent* event) {
    if (m_state == GameOver) return;
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
// ==================== 数据接口 ====================
void Game::setRunDeck(const QList<Card>& deck)       { m_runDeck = deck; }
QList<Card> Game::runDeck() const {
    QList<Card> all;
    all.append(m_drawPile);
    all.append(m_hand);
    all.append(m_discardPile);
    return all;
}
void Game::setRunBlessings(const QList<QString>& ids) { m_runBlessings = ids; }
void Game::setPlayerStartHP(int hp)                   { m_startHP = hp; }
void Game::setPlayerStartStrength(int str)            { m_startStrength = str; }
void Game::setEnemyId(const QString& id)        { m_enemyId = id; }
void Game::setInitialDeck(const QList<Card>& d) { m_initialDeck = d; }
// ==================== [临时注释] 存档 ====================
/*
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
    if (!file.open(QIODevice::ReadOnly)) {
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
*/