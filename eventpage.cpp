#include "eventpage.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

EventPage::EventPage(QWidget* parent) : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setAlignment(Qt::AlignCenter);
    m_layout->setSpacing(15);
    setStyleSheet("background: #1a1a2e; color: white; font-size: 14px;");
}

void EventPage::clearLayout() {
    QLayoutItem* item;
    while ((item = m_layout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
}

void EventPage::showBlessingOptions(const QList<QString>& ids,
                                    const QList<QPair<QString, QString>>& nameDescList)
{
    clearLayout();
    auto* title = new QLabel("选择一项祈福", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold;");
    m_layout->addWidget(title);

    for (int i = 0; i < ids.size() && i < nameDescList.size(); i++) {
        QString text = nameDescList[i].first + "\n" + nameDescList[i].second;
        auto* btn = new QPushButton(text, this);
        btn->setMinimumSize(300, 60);
        btn->setStyleSheet(
            "QPushButton { background: #2a2a4a; color: white; border-radius: 8px; "
            "padding: 10px; font-size: 13px; }"
            "QPushButton:hover { background: #4a4a7a; }");
        QString id = ids[i];
        connect(btn, &QPushButton::clicked, this, [this, id]() {
            emit blessingChosen(id);
        });
        m_layout->addWidget(btn);
    }
}

void EventPage::showCardPickOptions(const QList<QString>& ids,
                                    const QList<QPair<QString, QString>>& nameDescList)
{
    clearLayout();
    auto* title = new QLabel("选择一张牌加入牌组", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold;");
    m_layout->addWidget(title);

    for (int i = 0; i < ids.size() && i < nameDescList.size(); i++) {
        QString text = nameDescList[i].first + "\n" + nameDescList[i].second;
        auto* btn = new QPushButton(text, this);
        btn->setMinimumSize(300, 60);
        btn->setStyleSheet(
            "QPushButton { background: #2a2a4a; color: white; border-radius: 8px; "
            "padding: 10px; font-size: 13px; }"
            "QPushButton:hover { background: #4a4a7a; }");
        QString id = ids[i];
        connect(btn, &QPushButton::clicked, this, [this, id]() {
            emit cardChosen(id);
        });
        m_layout->addWidget(btn);
    }
}

void EventPage::showRestOption(int healAmount) {
    clearLayout();
    auto* title = new QLabel("休整", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold;");
    m_layout->addWidget(title);

    auto* btn = new QPushButton(QString("休息（回复 %1 HP）").arg(healAmount), this);
    btn->setMinimumSize(300, 60);
    btn->setStyleSheet(
        "QPushButton { background: #2a5a2a; color: white; border-radius: 8px; "
        "padding: 10px; font-size: 14px; }"
        "QPushButton:hover { background: #4a8a4a; }");
    connect(btn, &QPushButton::clicked, this, [this]() {
        emit restChosen();
    });
    m_layout->addWidget(btn);
}