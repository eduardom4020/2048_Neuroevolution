#ifndef GAME_H
#define GAME_H
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "util.h"

#define ROWS 4
#define COLS 4

#define FOUR_PERCENTILE 90

class Game
{
public:
    int game[ROWS][COLS] = {{0}};
    int copyGame[ROWS][COLS] = {{0}};
    int score = 0;
    int prevScore = 0;
    int error_count = 0;
    int maxSquare = 0;
    int monotonic_count = 0;

    bool gameIsNotOver = true;

//-------------------------------------------

    Game();
    ~Game();

    int randomTile();
    void emptyGame();
    void initGame();
    void saveGameCopy();
    void rotateClockwise();
    void rotateCounterClockwise();
    void flipHorizontally();
    int countZeros();
    void shiftRight();
    void shiftLeft();
    void shiftUp();
    void shiftDown();
    void printTile(int i, int j);
    void showGame();
    void setInput();
    void setInput(int input);
    floatv getGameState();
    floatv getNormalizedGameState();
    int getScore();
    int getGreaterSquare();

    bool stateEquals();
    bool isMonotonic();

#endif // GAME_H
};
