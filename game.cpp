#include "game.h"

Game::Game()
{

}

Game::~Game()
{

}

int Game::randomTile()
{
    int n = rand() % 100;

    // 90% = 2, 10% = 4
    if (n >= FOUR_PERCENTILE)
    {
        return 4;
    }

    return 2;
}


void Game::emptyGame()
{
    for (unsigned int i = 0; i < ROWS; ++i)
    {
        for (unsigned int j = 0; j < COLS; ++j)
        {
            game[i][j] = 0;
        }
    }
}


void Game::initGame()
{
    srand(time(NULL));
    emptyGame();

    score = 0;
    gameIsNotOver = true;


    int i, j;

    i = rand() % ROWS;
    j = rand() % COLS;

    game[i][j] = randomTile();


    i = rand() % ROWS;
    j = rand() % COLS;

    while (game[i][j] != 0)
    {
        i = rand() % ROWS;
        j = rand() % COLS;
    }

    game[i][j] = randomTile();
}


void Game::saveGameCopy()
{
    for (unsigned int i = 0; i < ROWS; ++i)
    {
        for (unsigned int j = 0; j < COLS; ++j)
        {
            copyGame[i][j] = game[i][j];
        }
    }
}


void Game::rotateClockwise()
{
    saveGameCopy();

    for (unsigned int i = 0; i < ROWS; ++i)
    {
        for (unsigned int j = 0; j < COLS; ++j)
        {
            game[i][j] = copyGame[COLS-1-j][i];
        }
    }
}


void Game::rotateCounterClockwise()
{
    saveGameCopy();

    for (unsigned int i = 0; i < ROWS; ++i)
    {
        for (unsigned int j = 0; j < COLS; ++j)
        {
            game[i][j] = copyGame[j][ROWS-1-i];
        }
    }
}


void Game::flipHorizontally()
{
    saveGameCopy();

    for (unsigned int i = 0; i < ROWS; ++i)
    {
        for (unsigned int j = 0; j < COLS; ++j)
        {
            game[i][j] = copyGame[i][COLS-1-j];
        }
    }
}


int Game::countZeros()
{
    int count = 0;

    for (unsigned int i = 0; i < ROWS; ++i)
    {
        for (unsigned int j = 0; j < COLS; ++j)
        {
            if (game[i][j] == 0)
            {
                ++count;
            }
        }
    }

    return count;
}


void Game::shiftRight()
{
    int moveCounter = 0;


    // tenta ir p direita o maximo possivel
    for (unsigned int i = 0; i < ROWS; ++i)
    {
        for (unsigned int j = COLS-1; j > 0; --j)
        {
            if (game[i][j] == 0)
            {
                int n = j;

                while (n > 0)
                {
                    if (game[i][j] == 0)
                    {
                        for (unsigned int k = j; k > 0; --k)
                        {
                            game[i][k] = game[i][k-1];

                            if (game[i][k-1] != 0)
                            {
                                ++moveCounter;
                            }
                        }
                        game[i][0] = 0;
                    }

                    --n;
                }
            }
        }
    }


    // soma se possivel
    for (unsigned int j = COLS-1; j > 0; --j)
    {
        for (unsigned int i = 0; i < ROWS; ++i)
        {
            if ((game[i][j] != 0) && (game[i][j] == game[i][j-1]))
            {
                game[i][j] += game[i][j];

                //soma a jogada atual aos scores
                score += game[i][j];

                // puxa os anteriores pra direita
                for (unsigned int k = j-1; k > 0; --k)
                {
                    game[i][k] = game[i][k-1];
                }

                game[i][0] = 0;


                ++moveCounter;
            }
        }
    }


    if (countZeros() == 0)
    {
        gameIsNotOver = false;
        return;
    }


    if (moveCounter > 0)
    {
        int x = rand() % ROWS;
        int y = rand() % COLS;

        while (game[x][y] != 0)
        {
            x = rand() % ROWS;
            y = rand() % COLS;
        }

        game[x][y] = randomTile();
    }
}


void Game::shiftLeft()
{
    flipHorizontally();
    shiftRight();
    flipHorizontally();
}


void Game::shiftUp()
{
    rotateClockwise();
    shiftRight();
    rotateCounterClockwise();
}


void Game::shiftDown()
{
    rotateCounterClockwise();
    shiftRight();
    rotateClockwise();
}


void Game::printTile(int i, int j)
{
//    printf("%d", game[i][j]);
    // cores customizadas
    if (game[i][j] == 0)  //preto
    {
        printf("\033[37m%d\033[0m", game[i][j]);
    }
    else if (game[i][j] == 2)
    {
        printf("\033[35m%d\033[0m", game[i][j]);
    }
    else if (game[i][j] == 4)
    {
        printf("\033[34m%d\033[0m", game[i][j]);
    }
    else if (game[i][j] == 8)
    {
        printf("\033[36m%d\033[0m", game[i][j]);
    }
    else if (game[i][j] == 16)
    {
        printf("\033[37m%d\033[0m", game[i][j]);
    }
    else if (game[i][j] == 32)
    {
        printf("\033[32m%d\033[0m", game[i][j]);
    }
    else if (game[i][j] == 64)
    {
        printf("\033[31m%d\033[0m", game[i][j]);
    }
    else if (game[i][j] == 128)
    {
        printf("\033[33m%d\033[0m", game[i][j]);
    }
    else if (game[i][j] == 2048)
    {
        printf("\033[33;47m%d\033[0m", game[i][j]);
    }
    else if (game[i][j] > 2048)
    {
        printf("\033[37;40m%d\033[0m", game[i][j]);
    }
    else if (game[i][j] > 128)
    {
        printf("\033[37;43m%d\033[0m", game[i][j]);
    }
}


void Game::showGame()
{
    printf("\n\n");

    for (unsigned int i = 0; i < ROWS; ++i)
    {
        for (unsigned int j = 0; j < COLS; ++j)
        {
            printf("\t");
            printTile(i, j);
        }
        printf("\n");
    }
}


void Game::setInput()
{
    int option;

    printf("\n\n\tSCORE: %d", score);
    printf("\n\nInput: ");

    scanf("%d", &option);
    // option = getche() - 48; // funcao getche definida la em cima

    switch (option)
    {
        case 4:
            shiftLeft();
            break;

        case 2:
            shiftDown();
            break;

        case 6:
            shiftRight();
            break;

        case 8:
            shiftUp();
            break;
    }
}

void Game::setInput(int input)
{
//    printf("\n\n\tSCORE: %d", score);
//    printf("\n\nInput: %d", input);

    switch (input)
    {
        case 4:
            shiftLeft();
            break;

        case 2:
            shiftDown();
            break;

        case 6:
            shiftRight();
            break;

        case 8:
            shiftUp();
            break;
    }
}

floatv Game::getGameState()
{
    floatv gameState;

    for (unsigned int i = 0; i < ROWS; ++i)
    {
        for (unsigned int j = 0; j < COLS; ++j)
        {
            gameState.push_back(game[i][j]);    //converteu a matriz game para o vetor gameState
        }
    }

    return gameState;
}

int Game::getScore()
{
    return score;
}
