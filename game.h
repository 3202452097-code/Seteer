#ifndef GAME_H
#define GAME_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QList>
#include <memory>
#include "Card.h"
#include "Entity.h"
#include "StringSpace.h"
#include "BattleContext.h"

class CardItem;

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

signals:
    void battleFinished(bool victory);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    QGraphicsScene* m_scene;
    QGraphicsRectItem* m_endTurnButton;
    QGraphicsTextItem* m_endTurnLabel;
    int m_turnCount = 0;
    Player m_player;
    std::unique_ptr<Enemy> m_enemy;           // ★
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
    QGraphicsTextItem* m_playerInfoText;
    QGraphicsTextItem* m_enemyInfoText;
    QGraphicsTextItem* m_pileInfoText;
    QGraphicsTextItem* m_stringSpaceText;

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
    void setEndTurnButtonVisible(bool visible);

    static constexpr int CARD_WIDTH = 120;
    static constexpr int CARD_OVERLAP = 60;
    static constexpr int HAND_Y_OFFSET = 200;
    static constexpr int PLAY_ZONE_Y = 200;

    friend class CardItem;
};
#endif // GAME_H