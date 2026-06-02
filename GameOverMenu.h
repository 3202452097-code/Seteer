#ifndef GAMEOVERMENU_H
#define GAMEOVERMENU_H
#include <QWidget>
class GameOverMenu : public QWidget
{
    Q_OBJECT
public:
    // ★ 不再需要 Game* 参数
    explicit GameOverMenu(bool isVictory, QWidget* parent = nullptr);
signals:
    void returnToMenu();  // ★ 发信号而不是直调 Game
private:
    void onReturnClicked();
};
#endif // GAMEOVERMENU_H