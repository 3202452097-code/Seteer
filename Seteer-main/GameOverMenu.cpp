#include "GameOverMenu.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
GameOverMenu::GameOverMenu(bool isVictory, QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(300, 180);
    setWindowFlags(Qt::Widget);  // ★ 作为子控件嵌入，不作为独立 Dialog
    QString bgColor = isVictory ? "rgba(0, 100, 0, 220)" : "rgba(100, 0, 0, 220)";
    setStyleSheet(QString("QWidget { background-color: %1; border-radius: 10px; }"
                          "QPushButton { background-color: #4a4a4a; color: white; border: none; "
                          "border-radius: 5px; padding: 8px; font-size: 14px; min-width: 120px; }"
                          "QPushButton:hover { background-color: #6a6a6a; }"
                          "QLabel { color: white; font-size: 20px; font-weight: bold; }")
                      .arg(bgColor));
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(20, 20, 20, 20);
    QLabel* titleLabel = new QLabel(isVictory ? "胜利！" : "失败...", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    QPushButton* returnButton = new QPushButton("返回主菜单", this);
    layout->addWidget(returnButton);
    connect(returnButton, &QPushButton::clicked, this, &GameOverMenu::onReturnClicked);
}
void GameOverMenu::onReturnClicked()
{
    emit returnToMenu();
}