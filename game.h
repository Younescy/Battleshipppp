#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

enum CellType {
    WATER,
    WATER_SHOT,
    BOAT,
    WRECK
};

struct Boat {
    int size;
    int row;
    int col;
    bool horizontal;
};

struct GameBoard {
    enum CellType** board;
    int size;
};

struct Game {
    struct GameBoard playerBoard;
    struct GameBoard computerBoard;
    struct Boat* playerBoats;
    struct Boat* computerBoats;
};


extern const int NUM_BOATS;
extern const int BOARD_SIZE;


struct Boat createBoat(int size, int row, int col, bool horizontal);
bool isBoatAlive(struct Boat* boat);
struct GameBoard createGameBoard(int size);
void placeBoat(struct GameBoard* board, struct Boat* boat);
bool canPlaceBoat(struct GameBoard* board, struct Boat* boat);
bool generateRandomOrientation();
void placeRandomBoats(struct GameBoard* board, struct Boat* boats, int* boatSizes, unsigned int seed);
void displayOpponentBoard(struct GameBoard* board, bool hideShips);
void performShot(struct GameBoard* board, int row, int col, bool isPlayer);
void playerTurn(struct Game* game);
void computerTurn(struct Game* game);
bool checkVictory(struct GameBoard* board);
void freeGameBoard(struct GameBoard* board);
void playGame();

#endif
