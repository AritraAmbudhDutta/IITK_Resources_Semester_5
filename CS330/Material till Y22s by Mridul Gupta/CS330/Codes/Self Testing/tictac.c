#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SIZE 3 // Tic-Tac-Toe is a 3x3 board

void printBoard(int board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
}

int checkWin(int board[SIZE][SIZE]) {
    // Check rows and columns
    for (int i = 0; i < SIZE; i++) {
        if ((board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != 0) || 
            (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != 0)) {
            return board[i][i];
        }
    }

    // Check diagonals
    if ((board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != 0) ||
        (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != 0)) {
        return board[1][1];
    }

    // Check if there are any empty spaces left
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                return 0; // Game is not finished
            }
        }
    }

    return -1; // Draw
}

void makeMove(int board[SIZE][SIZE], int player) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                board[i][j] = player;
                return;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 10) {
        printf("Invalid number of arguments\n");
        return 1;
    }

    int board[SIZE][SIZE];
    int index = 1;

    // Read the board state from command line arguments
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = atoi(argv[index++]);
        }
    }

    // Print current board state
    printf("Current board state:\n");
    printBoard(board);

    // Check if the game is already finished
    int result = checkWin(board);
    if (result == 1) {
        printf("Player 1 wins!\n");
        return 0;
    } else if (result == 2) {
        printf("Player 2 wins!\n");
        return 0;
    } else if (result == -1) {
        printf("It's a draw!\n");
        return 0;
    }

    // Make a move
    makeMove(board, 1); // Player 1 makes a move

    // Print updated board state
    printf("Updated board state after Player 1's move:\n");
    printBoard(board);

    // Prepare arguments for execl
    char *args[10];
    args[0] = "./print"; // Program name
    index = 1;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char *str = (char *)malloc(2);
            sprintf(str, "%d", board[i][j]);
            args[index++] = str;
        }
    }

    // for(int i = 0; i < 10; i++)
    //     printf("%s\n", args[i]);
    // args[9] = NULL;

    // Call player2 with updated board state
    if (execl("./print", args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], NULL) == -1) {
        perror("execl failed");
        return 1;
    }

    return 0;
}
