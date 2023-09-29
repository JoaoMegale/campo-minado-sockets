#include "common.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

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

    // readMatrix(arquivo, matriz);
}

void revealCell(int m_original[ROWS][COLS], int m_cliente[ROWS][COLS], int x, int y) {
    m_cliente[x][y] = m_original[x][y];
}

void addFlag(int m_cliente[ROWS][COLS], int x, int y) {
    m_cliente[x][y] = -3;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <v4/v6> <porta> -i <arquivo>\n", argv[0]);
        exit(1);
    }

    int server_socket;

    if (strcmp(argv[1], "v4") == 0) {
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
    } else if (strcmp(argv[1], "v6") == 0) {
        server_socket = socket(AF_INET6, SOCK_STREAM, 0);
    } else {
        fprintf(stderr, "Tipo de endereço inválido. Use 'v4' ou 'v6'\n");
        exit(1);
    }

    if (server_socket == -1) {
        perror("Erro ao criar socket do servidor");
        exit(1);
    }

    struct sockaddr_in server_addr;
    struct sockaddr_in6 server_addr6;

    if (strcmp(argv[1], "v4") == 0) {
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(atoi(argv[2]));
        server_addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        memset(&server_addr6, 0, sizeof(server_addr6));
        server_addr6.sin6_family = AF_INET6;
        server_addr6.sin6_port = htons(atoi(argv[2]));
        server_addr6.sin6_addr = in6addr_any;
    }

    if (strcmp(argv[1], "v4") == 0) {
        if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            perror("Erro ao vincular socket ao endereço");
            exit(1);
        }
    } else {
        if (bind(server_socket, (struct sockaddr *)&server_addr6, sizeof(server_addr6)) == -1) {
            perror("Erro ao vincular socket ao endereço");
            exit(1);
        }
    }

    if (listen(server_socket, 5) == -1) {
        perror("Erro ao aguardar por conexões");
        exit(1);
    }

    // matriz de jogo
    const char *filename = argv[4];
    int base_matrix[ROWS][COLS];
    readMatrix(filename, base_matrix);
    printMatrix(base_matrix);

    // matriz mostrada ao cliente
    int client_matrix[ROWS][COLS];
    for (int i=0;i<ROWS;i++) {
        for (int j=0;j<COLS;j++) {
            client_matrix[i][j] = -2;
        }
    }

    struct sockaddr_in6 client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket == -1) {
        perror("Erro ao aceitar conexão do cliente");
        exit(1);
    }

    char client_ip[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &(client_addr.sin6_addr), client_ip, INET6_ADDRSTRLEN);
    printf("client connected\n");

    struct Action client_msg;
    struct Action server_resp;
    while (1) {
        ssize_t bytes_received = recv(client_socket, &client_msg, sizeof(client_msg), 0);

        if (bytes_received == -1) {
            perror("Erro ao receber mensagem do cliente");
            exit(1);
        }

        if (bytes_received == 0) {
            printf("client disconnected\n");
            break;
        }

        // start
        else if (client_msg.type == 0) {
            server_resp.type = 3;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    server_resp.board[i][j] = client_matrix[i][j];
                }
            }
        }

        // reveal
        else if (client_msg.type == 1) {
            revealCell(base_matrix, client_matrix, client_msg.coordinates[0], client_msg.coordinates[1]);
            server_resp.type = 3;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    server_resp.board[i][j] = client_matrix[i][j];
                }
            }
        }

        else if (client_msg.type == 2) {
            addFlag(client_matrix, client_msg.coordinates[0], client_msg.coordinates[1]);
            server_resp.type = 3;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    server_resp.board[i][j] = client_matrix[i][j];
                }
            }
        }

        else if (client_msg.type == 4) {
            printf("flag removida\n");
        }

        else if (client_msg.type == 5) {
            printf("jogo resetado\n");
        }

        else if (client_msg.type == 7) {
            printf("client disconnected\n");
            break;
        }

        else {
            printf("comando desconhecido\n");
        }

        if (send(client_socket, &server_resp, sizeof(struct Action), 0) == -1) {
            perror("Erro ao enviar mensagem do servidor para o cliente");
            exit(1);
        }        
    }

    close(client_socket);
    close(server_socket);

    return 0;
}
