#ifndef GAME_H
#define GAME_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QList>
#include "card.h"
#include "entity.h"
#include "stringspace.h"
#include "BattleContext.h"
class CardItem;
class GameOverMenu;
class Game : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Game(QWidget *parent = nullptr);
    void initGame(int w, int h, const QString& title, const QIcon& icon = QIcon());
    void run();
    void saveGame();
    void loadGame();
    void returnToMainMenu();  //返回主菜单

    void onCardClicked(CardItem* item);
    void onCardDragged(CardItem* item, const QPointF& releasePos);
    QGraphicsScene* gameScene() const { return m_scene; }

    void showGameOverMenu();  //显示游戏结束菜单
protected:
    void mousePressEvent(QMouseEvent* event) override;
private:
    QGraphicsScene* m_scene;
    QGraphicsRectItem* m_endTurnButton;
    QGraphicsTextItem* m_endTurnLabel;
    // ── 战斗数据 ──
    Player m_player;
    Enemy m_enemy;
    StringSpace m_stringSpace;
    QList<Card> m_drawPile;
    QList<Card> m_hand;
    QList<Card> m_discardPile;
    QList<CardItem*> m_handItems;
    CardItem* m_selectedCard = nullptr;
    enum State { PlayerTurn, EnemyTurn, GameOver };
    State m_state = PlayerTurn;
    // UI 文字
    QGraphicsTextItem* m_playerInfoText;
    QGraphicsTextItem* m_enemyInfoText;
    QGraphicsTextItem* m_pileInfoText;
    QGraphicsTextItem* m_stringSpaceText;

    //菜单
    GameOverMenu* m_gameOverMenu = nullptr;

    // ── 内部方法 ──
    BattleContext buildContext() const;
    void initBattle();
    void startPlayerTurn();
    void endPlayerTurn();
    void enemyAction();
    void checkGameOver();
    void drawCards(int count);
    void playCard(int handIndex);
    void discardHand();
    void rearrangeHand();
    void updateUI();

    static constexpr int CARD_WIDTH = 120;
    static constexpr int CARD_OVERLAP = 60;
    static constexpr int HAND_Y_OFFSET = 200;
    static constexpr int PLAY_ZONE_Y = 200;

    friend class CardItem;
    friend class GameOverMenu;
};
#endif // GAME_H