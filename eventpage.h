#ifndef EVENTPAGE_H
#define EVENTPAGE_H

#include <QWidget>
#include <QList>
#include <QPair>

class QVBoxLayout;

class EventPage : public QWidget
{
    Q_OBJECT
public:
    explicit EventPage(QWidget* parent = nullptr);

    // 三种事件模式的 UI
    void showBlessingOptions(const QList<QString>& ids,
                             const QList<QPair<QString, QString>>& nameDescList);
    void showCardPickOptions(const QList<QString>& ids,
                             const QList<QPair<QString, QString>>& nameDescList);
    void showRestOption(int healAmount);

signals:
    void blessingChosen(const QString& id);
    void cardChosen(const QString& id);
    void restChosen();

private:
    void clearLayout();
    QVBoxLayout* m_layout;
};

#endif // EVENTPAGE_H