#include "game.h"
#include "carditem.h"
#include "blessingdata.h"
#include "enemydata.h"
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

// ==================== 构造 ====================
Game::Game(QWidget *parent)
    : QGraphicsView{parent},
    m_player(70, 3)
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

    m_stringSpace.onChanged = [this]() { updateUI(); };

    createUIElements();
}

// ==================== UI 元素创建（一次性） ====================
void Game::createUIElements()
{
    // ── 精灵 ──
    m_playerSprite = new SpriteItem();
    m_enemySprite = new SpriteItem();
    m_scene->addItem(m_playerSprite);
    m_scene->addItem(m_enemySprite);
    m_animator = new SpriteAnimator(this);

    // ── 玩家血条 ──
    m_playerHPBarBg = m_scene->addRect(0, 0, HP_BAR_W, HP_BAR_H,
                                       QPen(Qt::black), QBrush(QColor(40, 40, 40)));
    m_playerHPBarFg = m_scene->addRect(0, 0, 0, HP_BAR_H,
                                       Qt::NoPen, QBrush(QColor(0, 200, 0)));
    m_playerHPText = m_scene->addText("");
    m_playerHPText->setDefaultTextColor(Qt::white);
    m_playerHPText->setFont(QFont("Sans", 10, QFont::Bold));

    // ── 敌人血条 ──
    m_enemyHPBarBg = m_scene->addRect(0, 0, HP_BAR_W, HP_BAR_H,
                                      QPen(Qt::black), QBrush(QColor(40, 40, 40)));
    m_enemyHPBarFg = m_scene->addRect(0, 0, 0, HP_BAR_H,
                                      Qt::NoPen, QBrush(QColor(0, 200, 0)));
    m_enemyHPText = m_scene->addText("");
    m_enemyHPText->setDefaultTextColor(Qt::white);
    m_enemyHPText->setFont(QFont("Sans", 10, QFont::Bold));

    // ── 战斗数据 ──
    m_playerInfoText = m_scene->addText("");
    m_playerInfoText->setDefaultTextColor(Qt::white);
    m_playerInfoText->setFont(QFont("Sans", 11));
    m_enemyInfoText = m_scene->addText("");
    m_enemyInfoText->setDefaultTextColor(Qt::red);
    m_enemyInfoText->setFont(QFont("Sans", 11));

    // ── 状态 ──
    m_playerStatusText = m_scene->addText("");
    m_playerStatusText->setDefaultTextColor(QColor(255, 200, 100));
    m_playerStatusText->setFont(QFont("Sans", 10));
    m_enemyStatusText = m_scene->addText("");
    m_enemyStatusText->setDefaultTextColor(QColor(255, 150, 100));
    m_enemyStatusText->setFont(QFont("Sans", 10));

    // ── 意图 ──
    m_enemyIntentText = m_scene->addText("");
    m_enemyIntentText->setDefaultTextColor(QColor(255, 100, 100));
    m_enemyIntentText->setFont(QFont("Sans", 13, QFont::Bold));

    // ── 祈福 ──
    // m_blessingText = m_scene->addText("");
    // m_blessingText->setDefaultTextColor(QColor(180, 180, 255));
    // m_blessingText->setFont(QFont("Sans", 10));

    // ── 字符串空间 ──
    m_stringSpaceText = m_scene->addText("");
    m_stringSpaceText->setDefaultTextColor(QColor(255, 220, 100));
    m_stringSpaceText->setFont(QFont("Sans", 11));

    // ── 牌堆信息 ──
    m_pileInfoText = m_scene->addText("");
    m_pileInfoText->setDefaultTextColor(QColor(180, 180, 180));
    m_pileInfoText->setFont(QFont("Sans", 9));

    // ── 结束回合按钮 ──
    m_endTurnButton = m_scene->addRect(0, 0, 150, 40,
                                       QPen(Qt::white, 2),
                                       QBrush(QColor(80, 30, 30)));
    m_endTurnLabel = m_scene->addText("结束第1回合");
    m_endTurnLabel->setDefaultTextColor(Qt::white);
    m_endTurnLabel->setFont(QFont("Sans", 14));
    // 能量条（玩家）
    m_playerEnergyBarBg = m_scene->addRect(0,0, ENERGY_BAR_W, ENERGY_BAR_H,
                                           QPen(Qt::black), QBrush(QColor(40,40,40)));
    m_playerEnergyBarFg = m_scene->addRect(0,0, 0, ENERGY_BAR_H,
                                           Qt::NoPen, QBrush(QColor(255,215,0)));
    m_playerEnergyText = m_scene->addText("");
    m_playerEnergyText->setDefaultTextColor(Qt::white);
    m_playerEnergyText->setFont(QFont("Sans", 7, QFont::Bold));
}

// ==================== 初始化 ====================
void Game::initGame(int w, int h, const QString& title, const QIcon& icon) {
    Q_UNUSED(title);
    if (!icon.isNull()) setWindowIcon(icon);
    m_scene->setSceneRect(0, 0, w, h);

    QPixmap bgPixmap(":/resources/cards/bg.jpg");
    if (!bgPixmap.isNull()) {
        m_scene->setBackgroundBrush(QBrush(bgPixmap.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    } else {
        QLinearGradient g(0, 0, 0, h);
        g.setColorAt(0, QColor(30, 30, 50));
        g.setColorAt(1, QColor(20, 20, 40));
        m_scene->setBackgroundBrush(QBrush(g));
    }

    setAttribute(Qt::WA_AlwaysShowToolTips, true);       // 总是显示工具提示
    viewport()->setAttribute(Qt::WA_AlwaysShowToolTips, true); // 在 viewport 上也设置

    initBattle();
    viewport()->update();
    update();
}

// ==================== BattleContext 工厂 ====================
BattleContext Game::buildContext() const {
    BattleContext ctx;
    ctx.turnCount = m_turnCount;
    ctx.player      = const_cast<Player*>(&m_player);
    ctx.enemy       = m_enemy.get();
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
    m_dragonTransformed = false;
    // ★ 安全地获取初始阶段
    // if (m_enemy->getAI())
    //     m_lastEnemyPhase = m_enemy->getAI()->currentPhase();
    //

    // 卡组
    if (!m_initialDeck.isEmpty()) {
        m_drawPile = m_initialDeck;
        m_initialDeck.clear();
    } else if (!m_runDeck.isEmpty()) {
        m_drawPile = m_runDeck;
        m_runDeck.clear();
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_drawPile.begin(), m_drawPile.end(), g);

    // 玩家
    m_player.setHP(m_startHP);
    m_player.setBlock(0);
    m_player.setStrength(m_startStrength);
    m_player.restoreEnergy();

    // 敌人
    auto enemyPtr = EnemyDatabase::instance().createEnemy(m_enemyId);
    if (enemyPtr) {
        m_enemy = std::move(enemyPtr);
    } else {
        m_enemy = std::make_unique<Enemy>(40, std::make_unique<DoubaoAI>());
        m_enemy->setName("未知敌人");
    }
    m_enemy->setBlock(0);
    m_enemy->setStrength(0);

    // ★ 加载精灵图
    QPixmap playerPix(":/resources/sprite/player_idle.png");
    if (playerPix.isNull()) {
        playerPix = QPixmap(150, 200);
        playerPix.fill(Qt::gray);
    }
    m_playerSprite->setPixmap(playerPix.scaled(150, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_playerSpriteSize = QSizeF(m_playerSprite->pixmap().width(), m_playerSprite->pixmap().height());
    // ---- 敌人精灵 ----
    int maxEnemyW = 250;   // 默认最大宽度
    // 1. 尝试加载敌人专属图片 enemy_<id>.png
    QString specificPath = QString(":/resources/sprite/enemy_%1.png").arg(m_enemyId);
    QPixmap enemyPix(specificPath);
    // 2. 如果专属图片不存在，回退到通用 enemy_idle.png
    if (enemyPix.isNull()) {
        enemyPix.load(":/resources/sprite/enemy_idle.png");
    } else {
        // 加载成功，可针对不同敌人微调最大宽度
        if (m_enemyId == "dragon") {
            maxEnemyW = 400;
        } else if (m_enemyId == "linearalgebra") {
            maxEnemyW = 280;
        }
        // 未来在此扩展其他敌人的尺寸
    }
    // 3. 如果连通用 idle 图都加载失败，用纯色占位
    if (enemyPix.isNull()) {
        enemyPix = QPixmap(200, 250);
        enemyPix.fill(Qt::red);
    }
    QSize scaledSize = enemyPix.size().scaled(maxEnemyW, 500, Qt::KeepAspectRatio);
    m_enemySprite->setPixmap(enemyPix.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_enemySpriteSize = QSizeF(m_enemySprite->pixmap().width(), m_enemySprite->pixmap().height());
    m_playerSprite->setVisible(true);
    m_enemySprite->setVisible(true);
    // ---- 绑定受击动画 ----
    m_player.onHit = [this](int) { m_animator->playHit(m_playerSprite); };
    m_enemy->onHit = [this](int) { m_animator->playHit(m_enemySprite); };

    // 给玩家绑定
    m_player.onDamaged = [this](int dmg) {
        auto* text = m_scene->addText(QString("-%1").arg(dmg));
        text->setDefaultTextColor(Qt::red);
        text->setFont(QFont("Sans", 16, QFont::Bold));
        QPointF pos = m_playerSprite->pos() + QPointF(m_playerSpriteSize.width() / 2, -20);
        text->setPos(pos);
        // 动画：上飘 + 淡出
        auto* anim = new QPropertyAnimation(text, "pos");
        anim->setDuration(800);
        anim->setStartValue(pos);
        anim->setEndValue(pos + QPointF(0, -40));
        anim->start(QAbstractAnimation::DeleteWhenStopped);
        // 淡出
        auto* fade = new QGraphicsOpacityEffect;
        text->setGraphicsEffect(fade);
        auto* fadeAnim = new QPropertyAnimation(fade, "opacity");
        fadeAnim->setDuration(800);
        fadeAnim->setStartValue(1.0);
        fadeAnim->setEndValue(0.0);
        fadeAnim->start(QAbstractAnimation::DeleteWhenStopped);
        // 动画结束后删除文字
        connect(anim, &QPropertyAnimation::finished, [this, text]() {
            m_scene->removeItem(text);
            text->deleteLater();
        });
    };
    // 给敌人绑定（同理）
    m_enemy->onDamaged = [this](int dmg) {
        auto* text = m_scene->addText(QString("-%1").arg(dmg));
        text->setDefaultTextColor(Qt::yellow);
        text->setFont(QFont("Sans", 16, QFont::Bold));
        QPointF pos = m_enemySprite->pos() + QPointF(m_enemySpriteSize.width() / 2, -20);
        text->setPos(pos);
        auto* anim = new QPropertyAnimation(text, "pos");
        anim->setDuration(800);
        anim->setStartValue(pos);
        anim->setEndValue(pos + QPointF(0, -40));
        anim->start(QAbstractAnimation::DeleteWhenStopped);
        auto* fade = new QGraphicsOpacityEffect;
        text->setGraphicsEffect(fade);
        auto* fadeAnim = new QPropertyAnimation(fade, "opacity");
        fadeAnim->setDuration(800);
        fadeAnim->setStartValue(1.0);
        fadeAnim->setEndValue(0.0);
        fadeAnim->start(QAbstractAnimation::DeleteWhenStopped);
        connect(anim, &QPropertyAnimation::finished, [this, text]() {
            m_scene->removeItem(text);
            text->deleteLater();
        });
    };

    //字符串空间
    m_stringSpace.clear();
    // 设置字符串空间写入回调，触发玩家能力
    m_stringSpace.onAppended = [this](const QString&) {
        if (m_state == GameOver) return;
        BattleContext ctx = buildContext();
        ctx.attacker = &m_player;
        ctx.defender = m_enemy.get();
        QVariantMap eventData;
        eventData["text"] = QString(); // 暂时无内容
        m_player.triggerAbilities(Trigger::OnWriteString, eventData, ctx);
        // 可选敌人也触发
        // m_enemy->triggerAbilities(...);
    };

    m_state = PlayerTurn;
    BattleContext ctx = buildContext();
    m_enemy->decideIntent(ctx);
    startPlayerTurn();
    updateUI();
}

// ==================== 回合 ====================
void Game::startPlayerTurn() {
    setInputEnabled(true);
    BattleContext ctx = buildContext();
    m_player.onTurnStart(ctx);
    ctx.attacker = &m_player;
    ctx.defender = m_enemy.get();

    m_turnCount++;
    ctx.turnCount = m_turnCount;
    for (const QString& id : m_runBlessings) {
        const BlessingData* b = BlessingDatabase::instance().blessingById(id);
        if (b) for (auto& effect : b->effects) effect.execute(ctx);
    }
    drawCards(5);
    m_player.tickStatuses();
    m_enemy->tickStatuses();
    m_player.tickAbilities();
    m_enemy->tickAbilities();

    m_player.triggerAbilities(Trigger::OnTurnStart, {}, ctx);
    m_state = PlayerTurn;
    qInfo() << "--- 玩家回合开始 ---";
    updateUI();
}

void Game::endPlayerTurn() {
    qInfo() << "--- 玩家回合结束 ---";
    discardHand();

    checkEnemyPhase();

    m_state = EnemyTurn;
    enemyAction();
    updateUI();
}

void Game::checkEnemyPhase() {
    // 安全保护
    if (!m_enemy || !m_enemy->getAI()) return;

    int currentPhase = m_enemy->getAI()->currentPhase();
    // 阶段未变，直接返回
    if (currentPhase == m_lastEnemyPhase) return;

    // ═══ 龙转阶段换图 ═══
    if (m_enemyId == "dragon" && currentPhase == 2 && !m_dragonTransformed) {
        QPixmap newPix(":/resources/sprite/dragon_the_end.png");
        if (!newPix.isNull()) {
            int maxW = 400;   // 与初始龙保持一致
            QSize scaled = newPix.size().scaled(maxW, 500, Qt::KeepAspectRatio);
            m_enemySprite->setPixmap(newPix.scaled(scaled, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_enemySpriteSize = QSizeF(m_enemySprite->pixmap().width(), m_enemySprite->pixmap().height());
            updateUI();
        }
        m_dragonTransformed = true;
    }

    // ═══ 未来其他 Boss 的阶段图片切换可在此继续添加 ═══

    m_lastEnemyPhase = currentPhase;   // 更新缓存
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
// void Game::playCard(int handIndex) {
//     if (handIndex < 0 || handIndex >= m_hand.size()) return;
//     Card card = m_hand[handIndex];
//     const CardData* data = card.data();
//     if (!data) return;
//     if (data->cost > m_player.energy()) {
//         qInfo() << "能量不足！";
//         rearrangeHand();
//         return;
//     }
//     m_player.setEnergy(m_player.energy() - data->cost);
//     BattleContext ctx = buildContext();
//     ctx.attacker = &m_player;
//     ctx.defender = m_enemy.get();
//     m_player.triggerAbilities(Trigger::OnCardPlayed, {}, ctx);
//     for (auto& eff : data->effects) {
//         eff.execute(ctx);
//         if (m_enemy->isDead()) break;
//     }
//     CardItem* item = m_handItems.takeAt(handIndex);
//     QTimer::singleShot(0, this, [this, item]() {
//         m_scene->removeItem(item); delete item;
//     });
//     m_hand.removeAt(handIndex);
//     if (!data->exhaust) m_discardPile.append(card);
//     m_selectedCard = nullptr;
//     for (int i = 0; i < m_handItems.size(); i++) m_handItems[i]->setHandIndex(i);
//     rearrangeHand();
//     updateUI();
//     if (m_state != GameOver && m_enemy->isDead()) {
//         m_state = GameOver;
//         emit battleFinished(true);
//     }
// }

void Game::executeCardEffects(int handIndex)
{
    if (handIndex < 0 || handIndex >= m_hand.size()) return;
    Card card = m_hand[handIndex];
    const CardData* data = card.data();
    if (!data) return;
    BattleContext ctx = buildContext();
    ctx.attacker = &m_player;
    ctx.defender = m_enemy.get();
    m_player.triggerAbilities(Trigger::OnCardPlayed, {}, ctx);
    for (auto& eff : data->effects) {
        eff.execute(ctx);
        if (m_enemy->isDead()) break;
    }
    CardItem* item = m_handItems.takeAt(handIndex);
    QTimer::singleShot(0, this, [this, item]() {
        m_scene->removeItem(item); delete item;
    });
    m_hand.removeAt(handIndex);
    if (!data->exhaust) m_discardPile.append(card);
    m_selectedCard = nullptr;
    for (int i = 0; i < m_handItems.size(); i++) m_handItems[i]->setHandIndex(i);
    rearrangeHand();
    updateUI();
}
void Game::performEnemyEffects()
{
    BattleContext ctx = buildContext();
    m_enemy->onTurnStart(ctx);
    ctx.attacker = m_enemy.get();
    ctx.defender = &m_player;
    m_enemy->triggerAbilities(Trigger::OnTurnStart, {}, ctx);
    m_enemy->executeSpecialEffect(ctx);
    std::vector<Effect> effects = m_enemy->takePendingEffects();
    for (auto& eff : effects) {
        eff.execute(ctx);
        if (m_player.isDead()) break;
    }
    m_enemy->onTurnEnd(ctx);
    m_enemy->triggerAbilities(Trigger::OnTurnEnd, {}, ctx);
}

void Game::playCard(int handIndex)
{
    if (m_state != PlayerTurn || m_inputBlocked) return;
    const CardData* data = m_hand[handIndex].data();
    if (!data) return;
    if (data->cost > m_player.energy()) {
        rearrangeHand();
        return;
    }
    m_player.setEnergy(m_player.energy() - data->cost);
    executeCardEffects(handIndex);
    // 无论如何都播放攻击动画（视觉反馈）
    if (data->isAttack) {
        m_animator->playAttack(m_playerSprite, true);
    } else {
        m_animator->playAction(m_playerSprite);
    }
    // 如果敌人死亡，启动死亡动画（与攻击动画并行）
    if (m_enemy->isDead()) {
        m_animator->playDeath(m_enemySprite);
        // 连接死亡动画结束信号，切换游戏结束
        connect(m_animator, &SpriteAnimator::finished, this,
                [this](const QString &type) {
                    if (type != "Death") return;
                    disconnect(m_animator, &SpriteAnimator::finished, this, nullptr);
                    m_state = GameOver;
                    emit battleFinished(true);
                });
    }
}

// ==================== 弃牌 ====================
void Game::discardHand() {
    for (Card c : m_hand) m_discardPile.append(c);
    m_hand.clear();
    for (auto* item : m_handItems) { m_scene->removeItem(item); delete item; }
    m_handItems.clear();
    m_selectedCard = nullptr;
}

// ==================== 敌人 ====================
void Game::enemyAction() {
    if (m_state == GameOver) return;
    setInputEnabled(false);
    connect(m_animator, &SpriteAnimator::finished, this,
            [this](const QString &type) {
                if (type != "Attack") return;
                disconnect(m_animator, &SpriteAnimator::finished, this, nullptr);
                performEnemyEffects();
                if (m_player.isDead()) {
                    updateUI();
                    m_animator->playDeath(m_playerSprite);
                    connect(m_animator, &SpriteAnimator::finished, this,
                            [this](const QString &t) {
                                if (t != "Death") return;
                                disconnect(m_animator, &SpriteAnimator::finished, this, nullptr);
                                m_state = GameOver;
                                emit battleFinished(false);
                            });
                } else {
                    BattleContext ctx = buildContext();
                    m_enemy->decideIntent(ctx);
                    checkEnemyPhase();
                    startPlayerTurn();
                }
            });
    m_animator->playAttack(m_enemySprite, false);
}

void Game::playDeathThenEnd(bool playerWon)
{
    setInputEnabled(false);
    SpriteItem* dying = playerWon ? m_enemySprite : m_playerSprite;
    m_animator->cancelAll();
    disconnect(m_animator, &SpriteAnimator::finished, this, nullptr);
    connect(m_animator, &SpriteAnimator::finished, this,
            [this, playerWon](const QString &type) {
                if (type != "Death") return;
                disconnect(m_animator, &SpriteAnimator::finished, this, nullptr);
                m_state = GameOver;
                if (playerWon)
                    emit battleFinished(true);
                else
                    emit battleFinished(false);
            });
    m_animator->playDeath(dying);
}
void Game::checkGameOver()
{
    if (m_player.isDead() && m_state != GameOver) {
        m_state = GameOver;   // 防止重复
        playDeathThenEnd(false);
    } else if (m_enemy->isDead() && m_state != GameOver) {
        m_state = GameOver;
        playDeathThenEnd(true);
    }
}

// void Game::checkGameOver() {
//     if (m_player.isDead() && m_state != GameOver) {
//         m_state = GameOver;
//         emit battleFinished(false);
//     } else if (m_enemy->isDead() && m_state != GameOver) {
//         m_state = GameOver;
//         emit battleFinished(true);
//     }
// }


// ==================== CardItem 回调 ====================
void Game::onCardClicked(CardItem* item) {
    if (m_state == GameOver || m_inputBlocked) return;
    if (m_selectedCard == item) { m_selectedCard = nullptr; }
    else {
        if (m_selectedCard) rearrangeHand();
        m_selectedCard = item;
        QPointF p = item->scenePos();
        item->setPos(p.x(), p.y() - 20);
    }
    updateUI();
}
void Game::onCardDragged(CardItem* item, const QPointF& releasePos) {
    if (m_state == GameOver || m_inputBlocked) return;
    if (releasePos.y() < PLAY_ZONE_Y) playCard(item->handIndex());
    else { rearrangeHand(); m_selectedCard = nullptr; }
    updateUI();
}

// ==================== 手牌排列 ====================
void Game::rearrangeHand() {
    int n = m_handItems.size();
    if (n == 0) return;
    int totalWidth = (n - 1) * CARD_OVERLAP + CARD_WIDTH;
    int startX = qMax(10, (width() - totalWidth) / 2);
    int y = height() - HAND_Y_OFFSET;
    for (int i = 0; i < n; i++) {
        CardItem* item = m_handItems[i];
        if (item == m_selectedCard) continue;
        item->snapTo(QPointF(startX + i * CARD_OVERLAP, y));
        item->setZValue(i);
    }
}

//格挡显示方法
QPixmap Game::getShieldIcon() const {
    QPixmap pix(":/resources/icons/shield.png");
    if (!pix.isNull())
        return pix.scaled(ICON_SIZE, ICON_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    // 占位盾牌
    QPixmap placeholder(ICON_SIZE, ICON_SIZE);
    placeholder.fill(Qt::transparent);
    QPainter p(&placeholder);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor(100, 100, 255));
    // 在 16x16 内绘制一个小盾牌形状
    p.drawEllipse(2, 2, 12, 12);
    p.drawRect(6, 6, 4, 8);
    p.end();
    return placeholder;
}
QPixmap Game::getStatusIcon(StatusType type) const {
    QString path;
    QColor fallbackColor;
    switch (type) {
    case StatusType::Vulnerable:
        path = ":/resources/icons/status_vulnerable.png";
        fallbackColor = QColor(255, 80, 80);  // 红
        break;
    case StatusType::Weak:
        path = ":/resources/icons/status_weak.png";
        fallbackColor = QColor(80, 80, 255);   // 蓝
        break;
    default:
        // 未知状态，直接返回灰色
        QPixmap placeholder(ICON_SIZE, ICON_SIZE);
        placeholder.fill(Qt::gray);
        return placeholder;
    }
    QPixmap pix(path);
    if (!pix.isNull())
        return pix.scaled(ICON_SIZE, ICON_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    // 占位色块
    QPixmap placeholder(ICON_SIZE, ICON_SIZE);
    placeholder.fill(fallbackColor);
    return placeholder;
}
QPixmap Game::getAbilityIcon(const QString& abilityId) const {
    QString path = QString(":/resources/icons/ability_%1.png").arg(abilityId);
    QPixmap pix(path);
    if (!pix.isNull())
        return pix.scaled(ICON_SIZE, ICON_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    // 占位灰色方块
    QPixmap placeholder(ICON_SIZE, ICON_SIZE);
    placeholder.fill(QColor(180, 180, 180));
    return placeholder;
}
QPixmap Game::getStatIcon(const QString& statId) const {
    if (statId == "strength") {
        QPixmap pix(":/resources/icons/strength.png");
        if (!pix.isNull())
            return pix.scaled(ICON_SIZE, ICON_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        // 占位金色方块
        QPixmap placeholder(ICON_SIZE, ICON_SIZE);
        placeholder.fill(QColor(255, 215, 0));
        return placeholder;
    }
    // 其他未支持的统计
    QPixmap placeholder(ICON_SIZE, ICON_SIZE);
    placeholder.fill(Qt::gray);
    return placeholder;
}
//状态显示方法
void Game::updateStatusIcons() {
    // 清除旧图标
    auto clearIcons = [this](QList<IconItem>& icons) {
        for (auto& icon : icons) {
            if (icon.pixmapItem) m_scene->removeItem(icon.pixmapItem);
            if (icon.textItem) m_scene->removeItem(icon.textItem);
            delete icon.pixmapItem;
            delete icon.textItem;
        }
        icons.clear();
    };
    clearIcons(m_playerIcons);
    clearIcons(m_enemyIcons);
    // 也移除旧的盾牌（会在后面重新创建）
    if (m_playerShield) { m_scene->removeItem(m_playerShield); delete m_playerShield; m_playerShield = nullptr; }
    if (m_playerShieldText) { m_scene->removeItem(m_playerShieldText); delete m_playerShieldText; m_playerShieldText = nullptr; }
    if (m_enemyShield) { m_scene->removeItem(m_enemyShield); delete m_enemyShield; m_enemyShield = nullptr; }
    if (m_enemyShieldText) { m_scene->removeItem(m_enemyShieldText); delete m_enemyShieldText; m_enemyShieldText = nullptr; }
    // 计算精灵和血条位置（复用 updateUI 中的变量，但为了解耦我们可以重新计算）
    int playerBarX = m_playerBarX;
    int playerBarY = m_playerBarY;
    int enemyBarX = m_enemyBarX;
    int enemyBarY = m_enemyBarY;
    // ── 血量/格挡特殊处理（在 updateUI 里实现，这里只负责图标） → 我们将在下面分开处理
    // 现在绘制盾牌和图标栏
    auto drawShieldIfNeeded = [this](int barX, int barY, int block, QGraphicsPixmapItem*& shield, QGraphicsTextItem*& shieldText) {
        if (block > 0) {
            shield = m_scene->addPixmap(getShieldIcon());
            shield->setPos(barX + HP_BAR_W + 4, barY);
            shieldText = m_scene->addText(QString::number(block));
            shieldText->setDefaultTextColor(Qt::white);
            shieldText->setFont(QFont("Sans", 10, QFont::Bold));
            shieldText->setPos(barX + HP_BAR_W + 20, barY - 2);
        }
    };
    drawShieldIfNeeded(playerBarX, playerBarY, m_player.block(), m_playerShield, m_playerShieldText);
    drawShieldIfNeeded(enemyBarX, enemyBarY, m_enemy->block(), m_enemyShield, m_enemyShieldText);
    // 图标栏起始Y位置：血条下方 + 2 像素
    int playerIconY = playerBarY + HP_BAR_H + 2;
    int enemyIconY = enemyBarY + HP_BAR_H + 2;
    const int iconSize = ICON_SIZE;
    const int textHeight = 12;
    const int spacing = 6;
    const int maxWidth = HP_BAR_W; // 跟血条等宽
    auto placeIcons = [this, iconSize, textHeight, spacing, maxWidth](Entity* entity, int startX, int startY, QList<IconItem>& iconList) {
        // 先收集所有需要显示的条目
        struct Entry {
            enum Kind { Status, Ability, Strength };
            Kind kind;
            QString id;             // 状态类型/能力ID/"strength"
            int value = 0;         // 显示数字 (状态剩余回合/能力剩余回合/力量值)
            int duration = 0;      // 用于状态和能力
            QString tooltip;
        };
        QList<Entry> entries;
        // 状态
        for (const auto& s : entity->statuses()) {
            if (s.duration <= 0) continue;
            Entry e;
            e.kind = Entry::Status;
            e.id = (s.type == StatusType::Vulnerable) ? "Vulnerable" : (s.type == StatusType::Weak) ? "Weak" : "Unknown";
            e.value = s.duration;
            e.duration = s.duration;
            e.tooltip = (s.type == StatusType::Vulnerable) ? "受到伤害 x1.5" : (s.type == StatusType::Weak) ? "造成伤害 x0.75" : "";
            entries.append(e);
        }
        // 能力
        for (const auto& a : entity->abilities()) {
            const AbilityData* data = AbilityDatabase::instance().abilityById(a.abilityId);
            if (!data) continue;
            Entry e;
            e.kind = Entry::Ability;
            e.id = a.abilityId;
            e.value = a.remainingTurns; // 临时显示剩余回合，永久=0
            e.duration = a.remainingTurns;
            e.tooltip = data->desc;
            entries.append(e);
        }
        // 力量
        if (entity->strength() > 0) {
            Entry e;
            e.kind = Entry::Strength;
            e.id = "strength";
            e.value = entity->strength();
            e.tooltip = QString("力量: 攻击伤害 +%1").arg(entity->strength());  // 力量鼠标悬浮显示文本新增
            entries.append(e);
        }
        // 排列布局
        int x = startX;
        int y = startY;
        for (const auto& entry : entries) {
            if (x + iconSize > startX + maxWidth) {
                x = startX;
                y += iconSize + textHeight + spacing;
            }
            QPixmap pix;
            switch (entry.kind) {
            case Entry::Status: pix = getStatusIcon(entry.id == "Vulnerable" ? StatusType::Vulnerable : StatusType::Weak); break;
            case Entry::Ability: pix = getAbilityIcon(entry.id); break;
            case Entry::Strength: pix = getStatIcon("strength"); break;
            }
            auto* pixItem = new TooltipIcon(pix, entry.tooltip.isEmpty() ? "" : entry.tooltip);
            pixItem->setPos(x, y);
            pixItem->setZValue(100);
            m_scene->addItem(pixItem);
            IconItem iconItem;
            iconItem.pixmapItem = pixItem;
            // 添加右下角数字（临时能力/状态显示剩余回合，力量显示数值）
            if (entry.value > 0 && !(entry.kind == Entry::Ability && entry.value == 0)) { // 永久能力不显示数字
                auto* text = m_scene->addText(QString::number(entry.value));
                text->setDefaultTextColor(Qt::white);
                text->setFont(QFont("Sans", 8, QFont::Bold));
                text->setPos(x + iconSize - 6, y + iconSize - 10);
                iconItem.textItem = text;
            }
            iconList.append(iconItem);
            x += iconSize + spacing;
        }
    };
    placeIcons(&m_player, playerBarX, playerIconY, m_playerIcons);
    placeIcons(m_enemy.get(), enemyBarX, enemyIconY, m_enemyIcons);
}
QPixmap Game::getBlessingIcon(const QString& blessingId) const {
    QString path = QString(":/icons/blessing_%1.png").arg(blessingId);
    QPixmap pix(path);
    if (!pix.isNull())
        return pix.scaled(ICON_SIZE, ICON_SIZE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    // 占位：浅紫色方块
    QPixmap placeholder(ICON_SIZE, ICON_SIZE);
    placeholder.fill(QColor(180, 180, 255));
    return placeholder;
}
// ==================== UI 更新 ====================
void Game::updateUI() {
    int w = width();
    int h = height();
    // 统一的“地面”高度（精灵底部贴齐这条线）
    int groundY = h / 2 + 100;

    // ═══════════════════════════════════
    // 精灵位置
    // ═══════════════════════════════════
    // 玩家位置
    int playerX = 80;
    int playerY = groundY - m_playerSpriteSize.height();
    m_playerSprite->setPos(playerX, playerY);
    // 敌人位置
    int enemyX = w - 80 - m_enemySpriteSize.width();
    int enemyY = groundY - m_enemySpriteSize.height();
    m_enemySprite->setPos(enemyX, enemyY);
    // 血条统一放在 groundY 下方 10 像素
    int playerBarX = playerX + (m_playerSpriteSize.width() - HP_BAR_W) / 2;
    int playerBarY = groundY + 10;
    int enemyBarX = enemyX + (m_enemySpriteSize.width() - HP_BAR_W) / 2;
    int enemyBarY = groundY + 10;
    // 更新成员变量，供 updateStatusIcons 使用
    m_playerBarX = playerBarX;
    m_playerBarY = playerBarY;
    m_enemyBarX = enemyBarX;
    m_enemyBarY = enemyBarY;



    // ═══════════════════════════════════
    // 玩家血条
    // ═══════════════════════════════════
    m_playerHPBarBg->setPos(playerBarX, playerBarY);
    double playerRatio = (double)m_player.hp() / m_player.maxHp();
    playerRatio = qMax(0.0, qMin(1.0, playerRatio));
    QColor playerHPColor;
    if (m_player.block() > 0) {
        playerHPColor = QColor(50, 100, 255); // 蓝色
    } else {
        playerHPColor = playerRatio > 0.6 ? QColor(0, 200, 0)
                    : playerRatio > 0.3 ? QColor(200, 200, 0)
                                        : QColor(200, 0, 0);
    }
    m_playerHPBarFg->setBrush(QBrush(playerHPColor));
    m_playerHPBarFg->setRect(0, 0, HP_BAR_W * playerRatio, HP_BAR_H);
    m_playerHPBarFg->setPos(playerBarX, playerBarY);
    m_playerHPText->setPlainText(QString("HP: %1/%2").arg(m_player.hp()).arg(m_player.maxHp()));
    m_playerHPText->setPos(playerBarX + 4, playerBarY);

    // ═══════════════════════════════════
    // 敌人血条
    // ═══════════════════════════════════
    m_enemyHPBarBg->setPos(enemyBarX, enemyBarY);
    double enemyRatio = (double)m_enemy->hp() / m_enemy->maxHp();
    enemyRatio = qMax(0.0, qMin(1.0, enemyRatio));
    QColor enemyHPColor;
    if (m_enemy->block() > 0) {
        enemyHPColor = QColor(50, 100, 255); // 蓝色表示有格挡
    } else {
        enemyHPColor = enemyRatio > 0.6 ? QColor(0, 200, 0)
                      : enemyRatio > 0.3 ? QColor(200, 200, 0)
                                        : QColor(200, 0, 0);
    }
    m_enemyHPBarFg->setBrush(QBrush(enemyHPColor));
    m_enemyHPBarFg->setRect(0, 0, HP_BAR_W * enemyRatio, HP_BAR_H);
    m_enemyHPBarFg->setPos(enemyBarX, enemyBarY);
    m_enemyHPText->setPlainText(QString("HP: %1/%2").arg(m_enemy->hp()).arg(m_enemy->maxHp()));
    m_enemyHPText->setPos(enemyBarX + 4, enemyBarY);

    // ═══════════════════════════════════
    // 格挡 + 力量 + 能量（玩家）
    // ═══════════════════════════════════
    // QString playerInfo = QString("🛡️%1  💪%2  ⚡%3/%4")
    //                          .arg(m_player.block()).arg(m_player.strength())
    //                          .arg(m_player.energy()).arg(m_player.maxEnergy());
    // m_playerInfoText->setPlainText(playerInfo);
    // m_playerInfoText->setPos(playerBarX, playerBarY + HP_BAR_H + 4);

    // // ═══════════════════════════════════
    // // 格挡 + 力量（敌人）
    // // ═══════════════════════════════════
    // QString enemyInfo = QString("🛡️%1  💪%2")
    //                         .arg(m_enemy->block()).arg(m_enemy->strength());
    // m_enemyInfoText->setPlainText(enemyInfo);
    // m_enemyInfoText->setPos(enemyBarX, enemyBarY + HP_BAR_H + 4);

    // ═══════════════════════════════════
    // 状态
    // ═══════════════════════════════════
    updateStatusIcons();
    // auto fmtStatus = [](const QList<StatusInstance>& list) -> QString {
    //     QStringList parts;
    //     for (const auto& s : list) {
    //         if (s.duration <= 0) continue;
    //         switch (s.type) {
    //         case StatusType::Vulnerable: parts.append(QString("🔴易伤(%1)").arg(s.duration)); break;
    //         case StatusType::Weak:       parts.append(QString("🔵虚弱(%1)").arg(s.duration)); break;
    //         default: break;
    //         }
    //     }
    //     return parts.isEmpty() ? "" : parts.join(" ");
    // };
    // QString pStatus = fmtStatus(m_player.statuses());
    // QString eStatus = fmtStatus(m_enemy->statuses());
    // m_playerStatusText->setPlainText(pStatus);
    // m_playerStatusText->setPos(playerBarX, playerBarY + HP_BAR_H + 22);
    // m_enemyStatusText->setPlainText(eStatus);
    // m_enemyStatusText->setPos(enemyBarX, enemyBarY + HP_BAR_H + 22);

    // ═══════════════════════════════════
    // 敌人意图（精灵上方）
    // ═══════════════════════════════════
    m_enemyIntentText->setPlainText(m_enemy->intentDescription());
    QRectF intentRect = m_enemyIntentText->boundingRect();
    QPointF enemyPos = m_enemySprite->pos();
    m_enemyIntentText->setPos(enemyPos.x() + (m_enemySpriteSize.width() - intentRect.width()) / 2,
                              enemyPos.y() - intentRect.height() - 10);

    // ═══════════════════════════════════
    // 祈福（左上角）
    // ═══════════════════════════════════
    // QStringList blessingNames;
    // for (const QString& id : m_runBlessings) {
    //     const BlessingData* b = BlessingDatabase::instance().blessingById(id);
    //     if (b) blessingNames.append("✨" + b->name);
    // }
    // m_blessingText->setPlainText(blessingNames.isEmpty() ? "" : blessingNames.join("  "));
    // m_blessingText->setPos(20, 10);
    for (auto* icon : m_blessingIcons) {
        m_scene->removeItem(icon);
        delete icon;
    }
    m_blessingIcons.clear();
    int blessX = 20, blessY = 10;
    for (const QString& id : m_runBlessings) {
        const BlessingData* b = BlessingDatabase::instance().blessingById(id);
        if (!b) continue;
        QPixmap pix = getBlessingIcon(id);
        QString tooltip = QString("%1\n%2").arg(b->name, b->desc);
        auto* icon = new TooltipIcon(pix, tooltip);
        icon->setPos(blessX, blessY);
        icon->setZValue(100);
        m_scene->addItem(icon);
        m_blessingIcons.append(icon);
        blessX += ICON_SIZE + 4;  // 间距4像素
    }

    // ═══════════════════════════════════
    // 字符串空间
    // ═══════════════════════════════════
    QString ss = QString("字符串空间: \"%1\" (长度 %2)")
                     .arg(m_stringSpace.content()).arg(m_stringSpace.length());
    m_stringSpaceText->setPlainText(ss);
    m_stringSpaceText->setPos(w / 2 - 200, h - 230);

    // ═══════════════════════════════════
    // 牌堆信息
    // ═══════════════════════════════════
    QString pile = QString("📦抽:%1  🗑️弃:%2  ✋手:%3")
                       .arg(m_drawPile.size()).arg(m_discardPile.size()).arg(m_hand.size());
    m_pileInfoText->setPlainText(pile);
    m_pileInfoText->setPos(20, h - 150);

    // ═══════════════════════════════════
    // 结束回合按钮
    // ═══════════════════════════════════
    m_endTurnButton->setPos(w - 180, h - 100);
    m_endTurnLabel->setPos(w - 170, h - 95);
    m_endTurnLabel->setPlainText(QString("结束第 %1 回合").arg(m_turnCount));
    if (m_state == GameOver) {
        m_endTurnLabel->setPlainText(m_player.isDead() ? "失败" : "胜利");
        m_endTurnButton->setBrush(QColor(60, 60, 60));
    }

    //能量显示
    // 能量槽位置：血条右下角远处，往上一点避开盾牌
    int energyBarX = playerBarX + HP_BAR_W;   // 调整水平偏移
    int energyBarY = playerBarY - 50;               // 往上移，离血条稍远
    m_playerEnergyBarBg->setPos(energyBarX, energyBarY);
    m_playerEnergyBarBg->setRect(0, 0, ENERGY_BAR_W, ENERGY_BAR_H); // 确保背景矩形大小正确
    double energyRatio = (double)m_player.energy() / m_player.maxEnergy();
    int fillHeight = static_cast<int>(ENERGY_BAR_H * energyRatio);
    // 前景从顶部开始，高度为 fillHeight，y 坐标为背景顶部
    m_playerEnergyBarFg->setRect(0, 0, ENERGY_BAR_W, fillHeight);
    m_playerEnergyBarFg->setPos(energyBarX, energyBarY + (ENERGY_BAR_H - fillHeight)); // 底部对齐
    m_playerEnergyText->setPlainText(QString("%1/%2").arg(m_player.energy()).arg(m_player.maxEnergy()));
    // 文字居中
    QRectF fgRect = m_playerEnergyBarFg->rect();
    qreal textX = energyBarX + (ENERGY_BAR_W - m_playerEnergyText->boundingRect().width()) / 2;
    qreal textY = energyBarY + (ENERGY_BAR_H - m_playerEnergyText->boundingRect().height()) / 2;
    m_playerEnergyText->setPos(textX, textY);
}

void Game::setEndTurnButtonVisible(bool visible) {
    m_endTurnButton->setVisible(visible);
    m_endTurnLabel->setVisible(visible);
}



// ==================== 鼠标点击 ====================
void Game::setInputEnabled(bool enabled)
{
    m_inputBlocked = !enabled;
    m_endTurnButton->setVisible(enabled);
    m_endTurnLabel->setVisible(enabled);
}
void Game::mousePressEvent(QMouseEvent* event) {
    if (m_state == GameOver || m_inputBlocked) return;
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
    QList<Card> all; all.append(m_drawPile); all.append(m_hand); all.append(m_discardPile);
    return all;
}
void Game::setRunBlessings(const QList<QString>& ids) { m_runBlessings = ids; }
void Game::setPlayerStartHP(int hp)                   { m_startHP = hp; }
void Game::setPlayerStartStrength(int str)            { m_startStrength = str; }
void Game::setEnemyId(const QString& id)              { m_enemyId = id; }
void Game::setInitialDeck(const QList<Card>& d)       { m_initialDeck = d; }