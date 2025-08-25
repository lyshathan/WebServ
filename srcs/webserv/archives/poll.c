// server-poll.c - un petit serveur qui surveille ses sockets avec poll() pour accepter des demandes de connexion et relaie les messages de ses clients
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 4242  // le port de notre serveur

int create_server_socket(void);
void accept_new_connection(int server_socket, struct pollfd **poll_fds, int *poll_count, int *poll_size);
void read_data_from_socket(int i, struct pollfd **poll_fds, int *poll_count, int server_socket);
void add_to_poll_fds(struct pollfd *poll_fds[], int new_fd, int *poll_count, int *poll_size);
void del_from_poll_fds(struct pollfd **poll_fds, int i, int *poll_count);

int main(void)
{
    printf("---- SERVER ----\n\n");

    int server_socket;
    int status;

    // Pour surveiller les sockets clients :
    struct pollfd *poll_fds; // Tableau de descripteurs
    int poll_size; // Taille du tableau de descipteurs
    int poll_count; // Nombre actuel de descripteurs dans le tableau

    // Création de la socket du serveur
    server_socket = create_server_socket();
    if (server_socket == -1) {
        return (1);
    }

    // Écoute du port via la socket
    printf("[Server] Listening on port %d\n", PORT);
    status = listen(server_socket, 10);
    if (status != 0) {
        fprintf(stderr, "[Server] Listen error: %s\n", strerror(errno));
        return (3);
    }

    // Préparation du tableau des descripteurs de fichier pour poll()
    // On va commencer avec assez de place pour 5 fds dans le tableau,
    // on réallouera si nécessaire
    poll_size = 5;
    poll_fds = calloc(poll_size + 1, sizeof *poll_fds);
    if (!poll_fds) {
        return (4);
    }
    // Ajoute la socket du serveur au tableau
    // avec alerte si la socket peut être lue
    poll_fds[0].fd = server_socket;
    poll_fds[0].events = POLLIN;
    poll_count = 1;

    printf("[Server] Set up poll fd array\n");

    while (1) { // Boucle principale
        // Sonde les sockets prêtes (avec timeout de 2 secondes)
        status = poll(poll_fds, poll_count, 2000);
        if (status == -1) {
            fprintf(stderr, "[Server] Poll error: %s\n", strerror(errno));
            exit(1);
        }
        else if (status == 0) {
            // Aucun descipteur de fichier de socket n'est prêt
            printf("[Server] Waiting...\n");
            continue;
        }

        // Boucle sur notre tableau de sockets
        for (int i = 0; i < poll_count; i++) {
			if (! (poll_fds[i].revents & POLLIN)) {
            // if ((poll_fds[i].revents & POLLIN) != 1) {
                // La socket n'est pas prête à être lue
                // on s'arrête là et on continue la boucle
                continue ;
            }
            printf("[%d] Ready for I/O operation\n", poll_fds[i].fd);
            // La socket est prête à être lue !
            if (poll_fds[i].fd == server_socket) {
                // La socket est notre socket serveur qui écoute le port
                accept_new_connection(server_socket, &poll_fds, &poll_count, &poll_size);
            }
            else {
                // La socket est une socket client, on va la lire
                read_data_from_socket(i, &poll_fds, &poll_count, server_socket);
            }
        }
    }
    return (0);
}