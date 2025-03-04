#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <port> <username>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    char buffer[BUFFER_SIZE];
    fd_set readfds;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);   // Saisie clavier
        FD_SET(sock, &readfds);           // Messages du serveur

        int max_fd = (sock > STDIN_FILENO) ? sock : STDIN_FILENO;

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        // Envoyer un message
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';

            if (strcmp(buffer, "§") == 0) {
                printf("Fin de la saisie des messages.\n");
                break;
            }

            char formatted_msg[BUFFER_SIZE + 50];
            snprintf(formatted_msg, sizeof(formatted_msg), "# %s > %s\n", argv[3], buffer);
            send(sock, formatted_msg, strlen(formatted_msg), 0);
        }

        // Recevoir un message
        if (FD_ISSET(sock, &readfds)) {
            ssize_t valread = read(sock, buffer, BUFFER_SIZE - 1);
            if (valread <= 0) {
                printf("Server disconnected\n");
                break;
            }
            buffer[valread] = '\0';
            printf("%s", buffer);
        }
    }

    close(sock);
    return 0;
}
