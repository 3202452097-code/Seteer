#include "spriteanimator.h"
#include <QHash>

SpriteAnimator::SpriteAnimator(QObject *parent) : QObject(parent) {}

void SpriteAnimator::clearActive()
{
    // 停止所有正在运行的动画，但不要delete（父对象管理）
    for (auto& entry : m_animations) {
        if (entry.anim->state() == QAbstractAnimation::Running)
            entry.anim->stop();
    }
    m_animations.clear();
}

void SpriteAnimator::cancelAll()
{
    clearActive();
}

// 取消指定精灵上、匹配类型过滤器的动画
void SpriteAnimator::cancelAnimations(QGraphicsObject* target, const QString& typeFilter)
{
    for (int i = m_animations.size() - 1; i >= 0; --i) {
        auto& e = m_animations[i];
        if (e.target == target && (typeFilter.isEmpty() || e.type == typeFilter)) {
            if (e.anim->state() == QAbstractAnimation::Running)
                e.anim->stop();
            m_animations.removeAt(i);
        }
    }
}

// ---------- 攻击动画 ----------
void SpriteAnimator::playAttack(QGraphicsObject *sprite, bool facingRight)
{
    if (!sprite) return;
    // 取消该精灵的位置动画
    cancelAnimations(sprite, "Position");

    const int distance = 20;
    QPointF startPos = sprite->pos();
    QPointF offset = facingRight ? QPointF(distance, 0) : QPointF(-distance, 0);

    auto *forward = new QPropertyAnimation(sprite, "pos");
    forward->setDuration(100);
    forward->setStartValue(startPos);
    forward->setEndValue(startPos + offset);
    forward->setEasingCurve(QEasingCurve::OutQuad);

    auto *back = new QPropertyAnimation(sprite, "pos");
    back->setDuration(150);
    back->setStartValue(startPos + offset);
    back->setEndValue(startPos);
    back->setEasingCurve(QEasingCurve::OutBack);

    auto *group = new QSequentialAnimationGroup(this);
    group->addAnimation(forward);
    group->addAnimation(back);
    connect(group, &QSequentialAnimationGroup::finished, this, [this]() {
        emit finished("Attack");
    });
    group->start();   // KeepWhenStopped
    m_animations.append({sprite, "Position", group});
}

// ---------- 施法/防御动作 ----------
void SpriteAnimator::playAction(QGraphicsObject *sprite)
{
    if (!sprite) return;
    cancelAnimations(sprite, "Position");

    QPointF startPos = sprite->pos();

    auto *up = new QPropertyAnimation(sprite, "pos");
    up->setDuration(100);
    up->setStartValue(startPos);
    up->setEndValue(startPos + QPointF(0, -15));
    up->setEasingCurve(QEasingCurve::OutQuad);

    auto *down = new QPropertyAnimation(sprite, "pos");
    down->setDuration(100);
    down->setStartValue(startPos + QPointF(0, -15));
    down->setEndValue(startPos);
    down->setEasingCurve(QEasingCurve::OutBounce);

    // auto *scaleUp = new QPropertyAnimation(sprite, "scale");
    // scaleUp->setDuration(100);
    // scaleUp->setStartValue(1.0);
    // scaleUp->setEndValue(1.05);

    // auto *scaleDown = new QPropertyAnimation(sprite, "scale");
    // scaleDown->setDuration(100);
    // scaleDown->setStartValue(1.05);
    // scaleDown->setEndValue(1.0);

    auto *posGroup = new QSequentialAnimationGroup;
    posGroup->addAnimation(up);
    posGroup->addAnimation(down);

    // auto *scaleGroup = new QSequentialAnimationGroup;
    // scaleGroup->addAnimation(scaleUp);
    // scaleGroup->addAnimation(scaleDown);

    auto *parallel = new QParallelAnimationGroup(this);
    parallel->addAnimation(posGroup);
    // parallel->addAnimation(scaleGroup);
    connect(parallel, &QParallelAnimationGroup::finished, this, [this]() {
        emit finished("Action");
    });
    parallel->start();
    m_animations.append({sprite, "Position", parallel});
}

// ---------- 受击动画 ----------
void SpriteAnimator::playHit(QGraphicsObject *sprite)
{
    if (!sprite) return;
    // 取消该精灵的效果动画
    cancelAnimations(sprite, "Effect");

    auto *blink = new QPropertyAnimation(sprite, "opacity");
    blink->setDuration(100);
    blink->setKeyValues({{0.0, 1.0}, {0.5, 0.3}, {1.0, 1.0}});
    auto *blink2 = new QPropertyAnimation(sprite, "opacity");
    blink2->setDuration(100);
    blink2->setKeyValues({{0.0, 1.0}, {0.5, 0.3}, {1.0, 1.0}});
    auto *group = new QSequentialAnimationGroup(this);
    group->addAnimation(blink);
    group->addAnimation(blink2);
    connect(group, &QSequentialAnimationGroup::finished, this, [this]() {
        emit finished("Hit");
    });
    group->start();
    m_animations.append({sprite, "Effect", group});
}

// ---------- 死亡动画 ----------
void SpriteAnimator::playDeath(QGraphicsObject *sprite)
{
    if (!sprite) return;
    // 取消该精灵的所有动画（位置+效果）
    cancelAnimations(sprite, QString());

    // 设置旋转中心为精灵中心
    sprite->setTransformOriginPoint(sprite->boundingRect().center());

    auto *rotate = new QPropertyAnimation(sprite, "rotation");
    rotate->setDuration(300);
    rotate->setStartValue(0);
    rotate->setEndValue(90);
    rotate->setEasingCurve(QEasingCurve::InBack);

    auto *bounce = new QPropertyAnimation(sprite, "pos");
    bounce->setDuration(300);
    bounce->setStartValue(sprite->pos());
    bounce->setEndValue(sprite->pos() + QPointF(0, -10));
    bounce->setEasingCurve(QEasingCurve::OutBounce);

    auto *fade = new QPropertyAnimation(sprite, "opacity");
    fade->setDuration(600);
    fade->setStartValue(1.0);
    fade->setEndValue(0.0);
    fade->setEasingCurve(QEasingCurve::InQuad);

    auto *group = new QParallelAnimationGroup(this);
    group->addAnimation(rotate);
    group->addAnimation(bounce);
    group->addAnimation(fade);
    connect(group, &QParallelAnimationGroup::finished, this, [this]() {
        emit finished("Death");
    });
    group->start();
    m_animations.append({sprite, "Death", group});
}