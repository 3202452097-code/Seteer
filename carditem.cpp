#include "carditem.h"
#include "game.h"
#include <QPainter>
#include <QGraphicsScene>
CardItem::CardItem(const Card& card, int handIndex, Game* game, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent),
    m_card(card),
    m_handIndex(handIndex),
    m_game(game)
{
    const CardData* data = card.data();

    // 使用统一的卡牌尺寸（从 game 类获取）
    const int CARD_W = Game::CARD_WIDTH;    // 120
    const int CARD_He = Game::CARD_HEIGHT;   // 180（新增）

    QPixmap canvas(CARD_W, CARD_He);
    canvas.fill(QColor(60, 60, 80)); // 背景色

    QPainter p(&canvas);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    // 加载原图（如果有）
    QPixmap originalPix;
    if (data) {
        originalPix.load(data->imagePath);
    }

    if (!originalPix.isNull()) {
        // ★ 关键：使用 IgnoreAspectRatio 强制拉伸填满
        QPixmap scaled = originalPix.scaled(CARD_W, CARD_He,
                                            Qt::IgnoreAspectRatio,
                                            Qt::SmoothTransformation);
        p.drawPixmap(0, 0, scaled);
    }

    // 绘制文字（名称 + 费用）
    p.setPen(Qt::black);
    p.setFont(QFont("Sans", 12, QFont::Bold));
    QString label = data ? (data->name + "\n" + QString::number(data->cost) + "费") : "???";
    p.drawText(canvas.rect(), Qt::AlignCenter, label);

    p.end();

    setPixmap(canvas);
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
}
void CardItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    if (!m_isDragging) {
        setZValue(100);
        setScale(1.08);
    }

    // 显示卡牌描述新增
    const CardData* data = m_card.data();
    if (data && !data->desc.isEmpty()) {
        // 将场景坐标转换为全局屏幕坐标
        QPointF scenePos = event->scenePos();
        QPoint globalPos = m_game->mapToGlobal(m_game->mapFromScene(scenePos));
        QToolTip::showText(globalPos, data->desc, m_game);
    }
    //新增 end
    QGraphicsPixmapItem::hoverEnterEvent(event);
}
// card discribe 新增
void CardItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    const CardData* data = m_card.data();
    if (data && !data->desc.isEmpty()) {
        QPointF scenePos = event->scenePos();
        QPoint globalPos = m_game->mapToGlobal(m_game->mapFromScene(scenePos));
        QToolTip::showText(globalPos, data->desc, m_game);
    }

    QGraphicsPixmapItem::hoverMoveEvent(event);
}
//新增 end

void CardItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    if (!m_isDragging) {
        setZValue(m_handIndex);
        setScale(1.0);
    }
    QToolTip::hideText();//card discribe 新增
    QGraphicsPixmapItem::hoverLeaveEvent(event);
}
