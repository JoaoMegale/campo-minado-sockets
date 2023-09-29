#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 4
#define COLS 4

struct Action {
    int type;
    int coordinates[2];
    int board[4][4];
};

void printMatrix(int matrix[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if(matrix[i][j] == -1) {
                printf("* ");
            }
            else if (matrix[i][j] == -2) {
                printf("- ");
            }
            else if (matrix[i][j] == -3) {
                printf("> ");
            }
            else if (matrix[i][j] >= 0) {
                printf("%d ", matrix[i][j]);
            }
            else {
                printf("valor inválido na matriz.\n");
            }               
        }
        printf("\n");
    }
}