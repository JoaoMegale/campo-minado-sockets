#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 4
#define COLS 4

void printMatrix(int matrix[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if(matrix[i][j] == -1) {
                printf("*\t\t");
            }
            else if (matrix[i][j] == -2) {
                printf("-\t\t");
            }
            else if (matrix[i][j] == -3) {
                printf(">\t\t");
            }
            else if (matrix[i][j] >= 0) {
                printf("%d\t\t", matrix[i][j]);
            }
            else {
                printf("valor inválido na matriz.\n");
            }               
        }
        printf("\n");
    }
}