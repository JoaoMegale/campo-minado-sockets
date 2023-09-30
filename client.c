#include "common.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int updateStatusReveal (int mstatus[ROWS][COLS], int x, int y) {
    if (x<0 || y<0 || x>ROWS || y>COLS) {
        printf("error: invalid cell\n");
        return 0;
    }
    if (mstatus[x][y] == 1) {
        printf("error: cell already revealed\n");
        return 0; //erro
    }
    else {
        mstatus[x][y] = 1;
        return 1; //correto
    }
}

int updateStatusAddFlag (int mstatus[ROWS][COLS], int x, int y) {
    if (x<0 || y<0 || x>ROWS || y>COLS) {
        printf("error: invalid cell\n");
        return 0;
    }
    if (mstatus[x][y] == 1) {
        printf("error: cannot insert flag in revealed cell\n");
        return 0;
    }
    else if (mstatus[x][y] == -1) {
        printf("error: cell already has a flag\n");
        return 0;
    }
    else {
        mstatus[x][y] = -1;
        return 1;
    }
}

int updateStatusRemoveFlag (int mstatus[ROWS][COLS], int x, int y) {
    if (x<0 || y<0 || x>ROWS || y>COLS) {
        printf("error: invalid cell\n");
        return 0;
    }
    if (mstatus[x][y] != -1) {
        printf("error: cell has no flag to remove\n");
        return 0;
    }
    else {
        mstatus[x][y] = 0;
        return 1;
    }
}

void updateStatusReset (int mstatus[ROWS][COLS]) {
    for (int i=0;i<ROWS;i++) {
        for (int j=0;j<COLS;j++) {
            mstatus[i][j] = 0;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP> <porta>\n", argv[0]);
        exit(1);
    }

    int client_socket;

    if (strchr(argv[1], ':') != NULL) {
        // É um endereço IPv6
        client_socket = socket(AF_INET6, SOCK_STREAM, 0);
    } else {
        // É um endereço IPv4
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
    }

    if (client_socket == -1) {
        perror("Erro ao criar socket do cliente");
        exit(1);
    }

    struct sockaddr_in server_addr;
    struct sockaddr_in6 server_addr6;

    if (strchr(argv[1], ':') != NULL) {
        // É um endereço IPv6
        memset(&server_addr6, 0, sizeof(server_addr6));
        server_addr6.sin6_family = AF_INET6;
        server_addr6.sin6_port = htons(atoi(argv[2]));
        if (inet_pton(AF_INET6, argv[1], &(server_addr6.sin6_addr)) <= 0) {
            perror("Erro ao converter endereço IPv6");
            exit(1);
        }
    } else {
        // É um endereço IPv4
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(atoi(argv[2]));
        if (inet_pton(AF_INET, argv[1], &(server_addr.sin_addr)) <= 0) {
            perror("Erro ao converter endereço IPv4");
            exit(1);
        }
    }

    if (connect(client_socket, (struct sockaddr *)(strchr(argv[1], ':') != NULL ? &server_addr6 : &server_addr), (strchr(argv[1], ':') != NULL ? sizeof(server_addr6) : sizeof(server_addr)))) {
        perror("Erro ao conectar ao servidor");
        exit(1);
    }

    if (strchr(argv[1], ':') != NULL) {
        printf("Conectado ao servidor IPv6 %s na porta %s\n", argv[1], argv[2]);
    } else {
        printf("Conectado ao servidor IPv4 %s na porta %s\n", argv[1], argv[2]);
    }

    /*  
    matriz de status
     0 = celula nao marcada
     1 = celula revelada
    -1 = celula com flag
    */
    int status_matrix[ROWS][COLS];
    for (int i=0;i<ROWS;i++) {
        for (int j=0;j<COLS;j++) {
            status_matrix[i][j] = 0;
        }
    }

    struct Action client_msg;
    char client_input[1024];
    int has_error;
    while (1) {
        has_error = 0;
        printf("> ");
        fgets(client_input, sizeof(client_input), stdin);

        if (strcmp(client_input, "start\n") == 0) {
            client_msg.type = 0;
        }

        else if (strncmp(client_input, "reveal ", 7) == 0) {
            int x, y;
            if (sscanf(client_input + 7, "%d,%d", &x, &y) == 2) {
                if (updateStatusReveal(status_matrix, x, y) == 1) {
                    client_msg.type = 1;
                    client_msg.coordinates[0] = x;
                    client_msg.coordinates[1] = y;                  
                }
                else {
                    has_error = 1;
                }
            }
        }

        else if (strncmp(client_input, "flag ", 5) == 0) {
            int x, y;
            if (sscanf(client_input + 5, "%d,%d", &x, &y) == 2) {
                if (updateStatusAddFlag(status_matrix, x, y) == 1) {
                    client_msg.type = 2;
                    client_msg.coordinates[0] = x;
                    client_msg.coordinates[1] = y;
                }
                else {
                    has_error = 1;
                }
            }
        }

        else if (strncmp(client_input, "remove_flag ", 12) == 0) {
            int x, y;
            if (sscanf(client_input + 12, "%d,%d", &x, &y) == 2) {
                if (updateStatusRemoveFlag(status_matrix, x, y) == 1) {
                    client_msg.type = 4;
                    client_msg.coordinates[0] = x;
                    client_msg.coordinates[1] = y;
                }
                else {
                    has_error = 1;
                }
            }
        }

        else if (strcmp(client_input, "reset\n") == 0) {
            updateStatusReset(status_matrix);
            client_msg.type = 5;
        }

        else if (strcmp(client_input, "exit\n") == 0) {
            client_msg.type = 7;
            break;
        }

        else {
            has_error = 1;
            printf("comando desconhecido\n");
        }

        if (has_error == 0){
            ssize_t bytes_sent = send(client_socket, &client_msg, sizeof(client_msg), 0);
            if (bytes_sent == -1) {
                perror("Erro ao enviar mensagem para o servidor");
                exit(1);
            }
        }

        struct Action server_msg;
        // se houver algum erro, o cliente nao espera nenhuma resposta do servidor
        // e deve digitar um novo comando.
        if (has_error == 0) {

            ssize_t bytes_received = recv(client_socket, &server_msg, sizeof(server_msg), 0);
            if (bytes_received == -1) {
                perror("Erro ao receber mensagem do servidor");
                exit(1);
            } else if (bytes_received == 0) {
                printf("Conexão com o servidor encerrada\n");
                break;
            }

            if (server_msg.type == 3) {
                printMatrix(server_msg.board);
            }

            if (server_msg.type == 6) {
                printf("YOU WIN!\n");
                printMatrix(server_msg.board);
                break;
            }

            if (server_msg.type == 8) {
                printf("GAME OVER!\n");
                printMatrix(server_msg.board);
                break;
            }
        }
    }

    close(client_socket);

    return 0;
}
