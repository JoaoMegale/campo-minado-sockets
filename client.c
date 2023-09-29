#include "common.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

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

    struct Action client_msg;
    char client_input[1024];
    int unknown_request; // comando desconhecido
    while (1) {
        unknown_request = 0;
        printf("> ");
        fgets(client_input, sizeof(client_input), stdin);

        if (strcmp(client_input, "start\n") == 0) {
            client_msg.type = 0;
        }

        else if (strcmp(client_input, "reveal\n") == 0) {
            client_msg.type = 1;
        }

        else if (strcmp(client_input, "flag\n") == 0) {
            client_msg.type = 2;
        }

        else if (strcmp(client_input, "flag\n") == 0) {
            client_msg.type = 2;
        }

        else if (strcmp(client_input, "remove_flag\n") == 0) {
            client_msg.type = 4;
        }

        else if (strcmp(client_input, "reset\n") == 0) {
            client_msg.type = 5;
        }

        else if (strcmp(client_input, "exit\n") == 0) {
            client_msg.type = 7;
            break;
        }

        else {
            unknown_request = 1;
            printf("comando desconhecido\n");
        }

        if (unknown_request == 0){
            ssize_t bytes_sent = send(client_socket, &client_msg, sizeof(client_msg), 0);
            if (bytes_sent == -1) {
                perror("Erro ao enviar mensagem para o servidor");
                exit(1);
            }
        }
    }

    close(client_socket);

    return 0;
}
