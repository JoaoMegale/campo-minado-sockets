#pragma once

#define ROWS 4
#define COLS 4

void printMatrix(int matriz[ROWS][COLS]);

struct Action {
    int type;
    int coordinates[2];
    int board[4][4];
};