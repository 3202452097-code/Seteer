#ifndef STATUS_H
#define STATUS_H

#include <QList>

enum class StatusType {
    Vulnerable,   // 受到伤害 ×1.5
    Weak,         // 造成伤害 ×0.75
    Frail,        // 获得格挡 ×0.75（后续）
    StringLock,   // 字符串空间锁定（后续）
};

struct StatusInstance {
    StatusType type;
    int amount = 1;
    int duration = 0;
};

#endif // STATUS_H