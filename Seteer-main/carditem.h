#ifndef CARDITEM_H
#define CARDITEM_H
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QToolTip>
#include "Card.h"
class Game; // 前向声明
class CardItem : public QGraphicsPixmapItem {
public:
    CardItem(const Card& card, int handIndex, Game* game,
             QGraphicsItem* parent = nullptr);
    const Card& card() const { return m_card; }
    int handIndex() const { return m_handIndex; }
    void setHandIndex(int idx) { m_handIndex = idx; }
    void snapTo(const QPointF& pos);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;    // card discribe 新增
private:
    Card m_card;           // 轻量引用（只含 id）
    int m_handIndex;
    Game* m_game;
    QPointF m_originalPos;
    QPointF m_pressScenePos;
    bool m_isDragging = false;
    static constexpr double DRAG_THRESHOLD = 8.0;
};
#endif // CARDITEM_H