#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int port = atoi(argv[1]);

    // Création du socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    //serveur en mode écoute//
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server reachable on IP 0.0.0.0 port %d\n", port);

    int client_sockets[MAX_CLIENTS] = {0};
    fd_set readfds;
    int max_fd;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_fd = server_fd;

        // Ajouter les sockets clients
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &readfds);
                if (client_sockets[i] > max_fd) max_fd = client_sockets[i];
            }
        }

        // Selection de socket
        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            continue;
        }

        // Nouvelle connexion
        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("accept");
                continue;
            }

            // Ajouter le nouveau client
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    printf("New client connected\n");
                    break;
                }
            }
        }

        // Lire les données des clients
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];
            if (sd > 0 && FD_ISSET(sd, &readfds)) {
                char buffer[BUFFER_SIZE];
                ssize_t valread = read(sd, buffer, BUFFER_SIZE - 1);

                if (valread <= 0) {
                    // Déconnexion
                    close(sd);
                    client_sockets[i] = 0;
                    printf("Client disconnected\n");
                } else {
                    // Diffuser le message à tous les clients
                    buffer[valread] = '\0';
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        int dest_sd = client_sockets[j];
                        if (dest_sd > 0 && dest_sd != sd) {
                            send(dest_sd, buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
