#ifndef SPRITEANIMATOR_H
#define SPRITEANIMATOR_H

#include <QObject>
#include <QGraphicsObject>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QPainter>

// 自定义精灵项，继承 QGraphicsObject 以支持属性动画
class SpriteItem : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    explicit SpriteItem(QGraphicsItem *parent = nullptr)
        : QGraphicsObject(parent) {}

    void setPixmap(const QPixmap &pixmap) {
        m_pixmap = pixmap;
        update();
    }
    QPixmap pixmap() const { return m_pixmap; }

    QRectF boundingRect() const override {
        if (m_pixmap.isNull()) return QRectF();
        return QRectF(0, 0, m_pixmap.width(), m_pixmap.height());
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override {
        if (!m_pixmap.isNull())
            painter->drawPixmap(0, 0, m_pixmap);
    }

private:
    QPixmap m_pixmap;
};

class SpriteAnimator : public QObject
{
    Q_OBJECT
public:
    explicit SpriteAnimator(QObject *parent = nullptr);

    // 修改参数为 QGraphicsObject*（SpriteItem 是其子类）
    void playAttack(QGraphicsObject *sprite, bool facingRight);
    void playAction(QGraphicsObject *sprite);
    void playHit(QGraphicsObject *sprite);
    void playDeath(QGraphicsObject *sprite);
    void cancelAnimations(QGraphicsObject*, const QString& typeFilter);

    void cancelAll();

signals:
    void finished(const QString &type);

private:
    void clearActive();
    struct ActiveAnim {
        QGraphicsObject* target;
        QString type;               // "Position", "Effect", "Death"
        QAbstractAnimation* anim;
    };
    QList<ActiveAnim> m_animations;
};

#endif // SPRITEANIMATOR_H