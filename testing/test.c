// server.c - un micro-serveur qui accepte une connexion avant de s'arrêter
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT "4242" // le port de notre serveur
#define BACKLOG 10  // nombre max de demandes de connexion dans la file d'attente

int main(void)
{
    struct addrinfo hints;
    struct addrinfo *res;
    int socket_fd;
    int client_fd;
    int status;
    // sockaddr_storage est une structure qui n'est pas associé à
    // une famille particulière. Cela nous permet de récupérer
    // une adresse IPv4 ou IPv6
    struct sockaddr_storage client_addr;
    socklen_t addr_size;

    // on prépare l'adresse et le port pour la socket de notre serveur
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;        // IPv4 ou IPv6, indifférent
    hints.ai_socktype = SOCK_STREAM;    // Connexion TCP
    hints.ai_flags = AI_PASSIVE;        // Remplit l'IP automatiquement

    status = getaddrinfo(NULL, PORT, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return (1);
    }

    // on crée la socket, on a lie et on écoute dessus
    socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    status = bind(socket_fd, res->ai_addr, res->ai_addrlen);
    if (status != 0) {
        fprintf(stderr, "bind: %s\n", strerror(errno));
        return (2);
    }
    listen(socket_fd, BACKLOG);

    // on accept une connexion entrante
    addr_size = sizeof client_addr;
    client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_size);
    if (client_fd == -1) {
        fprintf(stderr, "accept: %s\n", strerror(errno));
        return (3);
    }
    printf("New connection! Socket fd: %d, client fd: %d\n", socket_fd, client_fd);

    // on est prêts à communiquer avec le client via le client_fd !

    return (0);
}
