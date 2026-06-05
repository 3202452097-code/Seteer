#ifndef GAME_H
#define GAME_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>
#include <QList>
#include <memory>
#include "Card.h"
#include "Entity.h"
#include "StringSpace.h"
#include "BattleContext.h"
#include "spriteanimator.h"



class CardItem;

class TooltipIcon;

class Game : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Game(QWidget *parent = nullptr);
    void initGame(int w, int h, const QString& title, const QIcon& icon = QIcon());
    void onCardClicked(CardItem* item);
    void onCardDragged(CardItem* item, const QPointF& releasePos);
    QGraphicsScene* gameScene() const { return m_scene; }

    void setRunDeck(const QList<Card>& deck);
    QList<Card> runDeck() const;
    void setRunBlessings(const QList<QString>& blessingIds);
    void setPlayerStartHP(int hp);
    void setPlayerStartStrength(int str);
    void setEnemyId(const QString& id);
    void setInitialDeck(const QList<Card>& deck);
    const Player& player() const { return m_player; }

    //位置控制
    int m_playerBarX = 0;
    int m_playerBarY = 0;
    int m_enemyBarX = 0;
    int m_enemyBarY = 0;

    //转阶段控制
    int m_lastEnemyPhase = 0;

signals:
    void battleFinished(bool victory);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    QGraphicsScene* m_scene;
    int m_turnCount = 0;
    Player m_player;
    std::unique_ptr<Enemy> m_enemy;
    StringSpace m_stringSpace;

    QList<Card> m_runDeck;
    QList<QString> m_runBlessings;
    int m_startHP = 70;
    int m_startStrength = 0;
    QList<Card> m_drawPile;
    QList<Card> m_hand;
    QList<Card> m_discardPile;
    QList<CardItem*> m_handItems;
    QString m_enemyId;
    QList<Card> m_initialDeck;
    CardItem* m_selectedCard = nullptr;
    enum State { PlayerTurn, EnemyTurn, GameOver };
    State m_state = PlayerTurn;

    // ═══════════════════════════════════
    // ★ 新 UI 元素
    // ═══════════════════════════════════

    // 精灵
    SpriteItem* m_playerSprite = nullptr;
    SpriteItem* m_enemySprite = nullptr;

    // 动画器
    SpriteAnimator* m_animator = nullptr;
    // 动画相关辅助
    void setInputEnabled(bool enabled);
    void executeCardEffects(int handIndex);
    void performEnemyEffects();
    void playDeathThenEnd(bool playerWon);
    bool m_inputBlocked = false;
    // 精灵动态尺寸记录（用于 UI 布局）
    QSizeF m_playerSpriteSize;
    QSizeF m_enemySpriteSize;

    // 血条（背景 + 前景）
    QGraphicsRectItem* m_playerHPBarBg = nullptr;
    QGraphicsRectItem* m_playerHPBarFg = nullptr;
    QGraphicsRectItem* m_enemyHPBarBg = nullptr;
    QGraphicsRectItem* m_enemyHPBarFg = nullptr;
    QGraphicsTextItem* m_playerHPText = nullptr;
    QGraphicsTextItem* m_enemyHPText = nullptr;

    // 战斗数据文字
    QGraphicsTextItem* m_playerInfoText = nullptr;  // 格挡+力量
    QGraphicsTextItem* m_enemyInfoText = nullptr;
    QGraphicsTextItem* m_playerStatusText = nullptr; // 状态
    QGraphicsTextItem* m_enemyStatusText = nullptr;

    // 意图
    QGraphicsTextItem* m_enemyIntentText = nullptr;

    // 祈福
    //QGraphicsTextItem* m_blessingText = nullptr;
    QList<TooltipIcon*> m_blessingIcons;
    QPixmap getBlessingIcon(const QString& blessingId) const;

    // 字符串空间
    QGraphicsTextItem* m_stringSpaceText = nullptr;

    // 牌堆信息
    QGraphicsTextItem* m_pileInfoText = nullptr;

    // 结束回合按钮
    QGraphicsRectItem* m_endTurnButton = nullptr;
    QGraphicsTextItem* m_endTurnLabel = nullptr;

    //能量显示
    QGraphicsRectItem* m_playerEnergyBarBg = nullptr;
    QGraphicsRectItem* m_playerEnergyBarFg = nullptr;
    QGraphicsTextItem* m_playerEnergyText = nullptr;

    // 布局常量
    static constexpr int SPRITE_W = 150;
    static constexpr int SPRITE_H = 200;
    static constexpr int HP_BAR_W = 200;
    static constexpr int HP_BAR_H = 18;
    static constexpr int CARD_WIDTH = 120;
    static constexpr int CARD_OVERLAP = 60;
    static constexpr int HAND_Y_OFFSET = 200;
    static constexpr int PLAY_ZONE_Y = 200;
    static constexpr int ICON_SIZE = 16;
    static constexpr int ENERGY_BAR_W = 40;   // 竖条宽度
    static constexpr int ENERGY_BAR_H = 40;   // 竖条高度

    // ═══════════════════════════════════

    BattleContext buildContext() const;
    void initBattle();
    void createUIElements();
    void startPlayerTurn();
    void endPlayerTurn();
    void enemyAction();
    void checkGameOver();
    void drawCards(int count);
    void playCard(int handIndex);
    void discardHand();
    void rearrangeHand();
    void updateUI();
    void setEndTurnButtonVisible(bool visible);

    friend class CardItem;
    struct IconItem {
        QGraphicsPixmapItem* pixmapItem = nullptr;
        QGraphicsTextItem* textItem = nullptr;
    };
    QList<IconItem> m_playerIcons;
    QList<IconItem> m_enemyIcons;
    // 盾牌相关
    QGraphicsPixmapItem* m_playerShield = nullptr;
    QGraphicsTextItem* m_playerShieldText = nullptr;
    QGraphicsPixmapItem* m_enemyShield = nullptr;
    QGraphicsTextItem* m_enemyShieldText = nullptr;
    void updateStatusIcons();
    QPixmap getStatusIcon(StatusType type) const;
    QPixmap getAbilityIcon(const QString& abilityId) const;
    QPixmap getStatIcon(const QString& statId) const; // 力量等
    QPixmap getShieldIcon() const;

    //转阶段
    void checkEnemyPhase();          // 检查敌人是否转阶段
    bool m_dragonTransformed = false; // 龙是否已变过身
};

class TooltipIcon : public QGraphicsPixmapItem {
public:
    TooltipIcon(const QPixmap& pix, const QString& tooltip, QGraphicsItem* parent = nullptr)
        : QGraphicsPixmapItem(pix, parent), m_tooltipText(tooltip)
    {
        setAcceptHoverEvents(true);
    }

    ~TooltipIcon() {
        removeTooltip();
    }

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override {
        if (!m_tooltipText.isEmpty() && !m_tooltipItem) {
            // 创建文字
            QGraphicsTextItem* textItem = new QGraphicsTextItem(m_tooltipText);
            textItem->setDefaultTextColor(Qt::white);
            textItem->setFont(QFont("Sans", 10));
            QRectF textRect = textItem->boundingRect();

            // 创建背景矩形（稍微大一圈）
            QGraphicsRectItem* rectItem = new QGraphicsRectItem(textRect.adjusted(-4, -2, 4, 2));
            rectItem->setBrush(QColor(0, 0, 0, 180));  // 半透明黑
            rectItem->setPen(Qt::NoPen);
            rectItem->setZValue(200);

            // 将文字设为背景的子项，并调整位置
            textItem->setParentItem(rectItem);
            textItem->setPos(4 - textRect.left(), 2 - textRect.top());

            // 定位整个 tooltip
            rectItem->setPos(event->scenePos() + QPointF(12, -12));

            if (scene()) scene()->addItem(rectItem);
            m_tooltipItem = rectItem;
        }
        QGraphicsPixmapItem::hoverEnterEvent(event);
    }

    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override {
        if (m_tooltipItem) {
            m_tooltipItem->setPos(event->scenePos() + QPointF(12, -12));
        }
        QGraphicsPixmapItem::hoverMoveEvent(event);
    }

    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override {
        removeTooltip();
        QGraphicsPixmapItem::hoverLeaveEvent(event);
    }

private:
    void removeTooltip() {
        if (m_tooltipItem) {
            if (m_tooltipItem->scene()) {
                m_tooltipItem->scene()->removeItem(m_tooltipItem);
            }
            delete m_tooltipItem;  // 会自动删除子项 textItem
            m_tooltipItem = nullptr;
        }
    }

    QString m_tooltipText;
    QGraphicsRectItem* m_tooltipItem = nullptr;
};
#endif //GAME_H