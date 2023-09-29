#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 4
#define COLS 4

// Função para ler uma matriz 4x4 de um arquivo
int readMatrix(const char *filename, int matrix[ROWS][COLS]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo %s\n", filename);
        return 0; // Indica falha
    }

    // Ler a matriz do arquivo
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (fscanf(file, "%d,", &matrix[i][j]) != 1) {
                printf("Erro ao ler a matriz do arquivo.\n");
                fclose(file);
                return 0; // Indica falha
            }
        }
    }

    fclose(file);
    return 1; // Indica sucesso
}

// Função para imprimir a matriz
void printMatrix(int matrix[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <nome_do_arquivo>\n", argv[0]);
        return 1; // Indica erro
    }

    const char *filename = argv[1];
    int matrix[ROWS][COLS];

    if (!readMatrix(filename, matrix)) {
        return 1; // Indica erro
    }

    printf("Matriz lida do arquivo:\n");
    printMatrix(matrix);

    return 0; // Indica sucesso
}
