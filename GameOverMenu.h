#ifndef GAMEOVERMENU_H
#define GAMEOVERMENU_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class Game;

class GameOverMenu : public QWidget
{
    Q_OBJECT

public:
    explicit GameOverMenu(Game* game, bool isVictory, QWidget* parent = nullptr);

private slots:
    void onReturnToMenu();

private:
    Game* m_game;
};

#endif // GAMEOVERMENU_H