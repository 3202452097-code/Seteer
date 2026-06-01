#include "carditem.h"
#include "game.h"
#include <QPainter>
#include <QGraphicsScene>
CardItem::CardItem(const Card& card, int handIndex, Game* game,
                   QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent),
    m_card(card),
    m_handIndex(handIndex),
    m_game(game)
{
    // ── 从数据库获取卡牌数据 ──
    const CardData* data = card.data();
    QPixmap pix;
    if (data) {
        pix.load(data->imagePath);
    }
    // 图片加载失败 → 占位符
    if (pix.isNull()) {
        pix = QPixmap(120, 180);
        pix.fill(QColor(60, 60, 80));
        QPainter p(&pix);
        p.setPen(Qt::white);
        p.setFont(QFont("Sans", 12, QFont::Bold));
        QString label = data
                            ? (data->name + "\n" + QString::number(data->cost) + "费")
                            : "???";
        p.drawText(pix.rect(), Qt::AlignCenter, label);
        p.end();
    } else {
        pix = pix.scaled(120, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    setPixmap(pix);
    setAcceptHoverEvents(true);
    setCursor(Qt::OpenHandCursor);
}
void CardItem::snapTo(const QPointF& pos) {
    m_originalPos = pos;
    setPos(pos);
}
void CardItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    m_pressScenePos = event->scenePos();
    m_originalPos   = scenePos();
    m_isDragging    = false;
    setCursor(Qt::ClosedHandCursor);
    event->accept();
}
void CardItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (!m_isDragging) {
        double dist = QLineF(m_pressScenePos, event->scenePos()).length();
        if (dist > DRAG_THRESHOLD) {
            m_isDragging = true;
            setZValue(200);
        } else {
            return;
        }
    }
    QPointF delta = event->scenePos() - m_pressScenePos;
    setPos(m_originalPos + delta);
    event->accept();
}
void CardItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    setCursor(Qt::OpenHandCursor);
    if (m_isDragging) {
        m_game->onCardDragged(this, scenePos());
    } else {
        setPos(m_originalPos);
        m_game->onCardClicked(this);
    }
    m_isDragging = false;
    event->accept();
}
void CardItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    if (!m_isDragging) {
        setZValue(100);
        setScale(1.08);
    }
    QGraphicsPixmapItem::hoverEnterEvent(event);
}
void CardItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    if (!m_isDragging) {
        setZValue(m_handIndex);
        setScale(1.0);
    }
    QGraphicsPixmapItem::hoverLeaveEvent(event);
}
