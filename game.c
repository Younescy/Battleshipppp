#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>


const int NUM_BOATS = 5;
const int BOARD_SIZE = 10;
/** Function to create a boat
 */
struct Boat createBoat(int size, int row, int col, bool horizontal) {
    struct Boat boat;
    boat.size = size;
    boat.row = row;
    boat.col = col;
    boat.horizontal = horizontal;
    return boat;
}

/** Function to check if a boat is still alive */
bool isBoatAlive(struct Boat* boat) {
    return boat->size > 0;
}

/** Function to create a game board*/
struct GameBoard createGameBoard(int size) {
    struct GameBoard board;
    board.size = size;

    /** Memory allocation for the two-dimensional array*/
    board.board = (enum CellType**)malloc(size * sizeof(enum CellType*));
    for (int i = 0; i < size; ++i) {
        board.board[i] = (enum CellType*)malloc(size * sizeof(enum CellType));
        for (int j = 0; j < size; ++j) {
            board.board[i][j] = WATER; /** Initialize all cells to water */
        }
    }

    return board;
}

/** Function to place a boat on the board */
void placeBoat(struct GameBoard* board, struct Boat* boat) {
    for (int i = 0; i < boat->size; ++i) {
        if (boat->horizontal) {
            board->board[boat->row][boat->col + i] = BOAT;
        } else {
            board->board[boat->row + i][boat->col] = BOAT;
        }
    }
}

/** Function to check if a boat can be placed at a given position */
bool canPlaceBoat(struct GameBoard* board, struct Boat* boat) {
    for (int i = 0; i < boat->size; ++i) {
        if (boat->horizontal) {
            if (board->board[boat->row][boat->col + i] != WATER) {
                return false;
            }
        } else {
            if (board->board[boat->row + i][boat->col] != WATER) {
                return false;
            }
        }
    }
    return true;
}

/** Function to generate a random orientation (horizontal or vertical)*/
bool generateRandomOrientation() {
    return rand() % 2 == 0;
}

/** Function to place boats randomly with a specified seed */
void placeRandomBoats(struct GameBoard* board, struct Boat* boats, int* boatSizes, unsigned int seed) {
    srand(seed);

    for (int i = 0; i < NUM_BOATS; ++i) {
        struct Boat boat;
        do {
            boat.size = boatSizes[i];
            boat.horizontal = generateRandomOrientation();
            if (boat.horizontal) {
                boat.row = rand() % board->size;
                boat.col = rand() % (board->size - boat.size + 1);
            } else {
                boat.row = rand() % (board->size - boat.size + 1);
                boat.col = rand() % board->size;
            }
        } while (!canPlaceBoat(board, &boat));

        boats[i] = boat;
        placeBoat(board, &boat);
    }
}

/** Function to display the opponent's board (with or without hiding ships) */
void displayOpponentBoard(struct GameBoard* board, bool hideShips) {
    printf("   ");
    for (int i = 0; i < board->size; ++i) {
        printf("%c ", 'A' + i);
    }
    printf("\n");

    for (int i = 0; i < board->size; ++i) {
        if (i <= 8) {
            printf(" %d ", i + 1);
        } else {
            printf("%d ", i + 1);
        }
        for (int j = 0; j < board->size; ++j) {
            char displayChar = ' ';
            if (board->board[i][j] == WATER_SHOT) {
                displayChar = '*';
            } else if (board->board[i][j] == WRECK) {
                displayChar = 'X';
            } else if (!hideShips && board->board[i][j] == BOAT) {
                displayChar = ' ';
            } else if (hideShips && board->board[i][j] == BOAT) {
                displayChar = ' ';
            }
            printf("%c ", displayChar);
        }
        printf("\n");
    }
}

/** Function to perform a shot on the board*/
void performShot(struct GameBoard* board, int row, int col, bool isPlayer) {
    if (board->board[row][col] == BOAT) {
        board->board[row][col] = WRECK;
        printf("Hit!\n");
    } else if (board->board[row][col] == WATER) {
        board->board[row][col] = WATER_SHOT;
        printf("Miss.\n");
    } else {
        printf("Already shot here.\n");
    }

    /** Use displayOpponentBoard to display the opponent's board with the ships */
    displayOpponentBoard(board, isPlayer);
}

/** Function to perform a player's turn */
void playerTurn(struct Game* game) {
    printf("Player's turn.\n");
    displayOpponentBoard(&(game->computerBoard), true);

    char input[4];  /** Increase the size of the input to account for '\0' */
    printf("Where do you want to shoot? (e.g., A1)\n");
    scanf("%3s", input);

    /** Add a condition to handle coordinates like A10 */
    int row, col;
    if (strlen(input) == 2) {
        row = input[1] - '1';
        col = input[0] - 'A';
    } else if (strlen(input) == 3) {
        row = (input[1] - '0') * 10 + (input[2] - '0') - 1;
        col = input[0] - 'A';
    } else {
        printf("Invalid coordinates. Please try again. (e.g., A1)\n");
        return;
    }

    /** 
     * Ask for coordinates again if they are invalid 
     */
    while (strlen(input) > 3 || row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
        printf("Invalid coordinates. Please try again. (e.g., A1)\n");
        scanf("%3s", input);

        if (strlen(input) == 2) {
            row = input[1] - '1';
            col = input[0] - 'A';
        } else if (strlen(input) == 3) {
            row = (input[1] - '0') * 10 + (input[2] - '0') - 1;
            col = input[0] - 'A';
        } else {
            printf("Invalid coordinates. Please try again. (e.g., A1)\n");
            return;
        }
    }

    performShot(&(game->computerBoard), row, col, true);
}

/** Function to perform a computer's turn */
void computerTurn(struct Game* game) {
    printf("Computer's turn.\n");

    int row, col;
    do {
        row = rand() % BOARD_SIZE;
        col = rand() % BOARD_SIZE;
    } while (game->playerBoard.board[row][col] == WATER_SHOT);

    performShot(&(game->playerBoard), row, col, false);
}

/** Function to check for victory */
bool checkVictory(struct GameBoard* board) {
    for (int i = 0; i < board->size; ++i) {
        for (int j = 0; j < board->size; ++j) {
            if (board->board[i][j] == BOAT) {
                return false;
            }
        }
    }
    return true;
}

/** Function to free the memory of a game board*/
void freeGameBoard(struct GameBoard* board) {
    for (int i = 0; i < board->size; ++i) {
        free(board->board[i]);
    }
    free(board->board);
}

/** Main function to sequence the game */
void playGame() {
    /** Game initialization */
    struct Game game;
    game.playerBoats = (struct Boat*)malloc(NUM_BOATS * sizeof(struct Boat));
    game.computerBoats = (struct Boat*)malloc(NUM_BOATS * sizeof(struct Boat));

    /** Declaration and initialization of the boatSizes array */
    int boatSizes[NUM_BOATS];
    boatSizes[0] = 2;
    boatSizes[1] = 3;
    boatSizes[2] = 3;
    boatSizes[3] = 4;
    boatSizes[4] = 5;

    /** Seeds for random number generation */
    unsigned int seedPlayer = (unsigned int)time(NULL);
    unsigned int seedComputer = (unsigned int)time(NULL) + 1;  /** Add 1 to get a different seed */

    /** Place boats randomly for the player */
    game.playerBoard = createGameBoard(BOARD_SIZE);
    placeRandomBoats(&(game.playerBoard), game.playerBoats, boatSizes, seedPlayer);

  /** Place boats randomly for the computer */
    game.computerBoard = createGameBoard(BOARD_SIZE);
    placeRandomBoats(&(game.computerBoard), game.computerBoats, boatSizes, seedComputer);

    /** Beginning of the game */
    while (true) {
        /** Player Turn*/
        playerTurn(&game);
        if (checkVictory(&(game.computerBoard))) {
            printf("Congrats buddy ! U won Mr grignon :D\n");
            break;
        }

        /** COmputer turn  */
        computerTurn(&game);
        if (checkVictory(&(game.playerBoard))) {
            printf("HOW DARE U LOSE AGAINST A DUMB COMPUTER!\n");
            break;
        }
    }

    /** free allocated memory */
    freeGameBoard(&(game.playerBoard));
    freeGameBoard(&(game.computerBoard));
    free(game.playerBoats);
    free(game.computerBoats);
}

