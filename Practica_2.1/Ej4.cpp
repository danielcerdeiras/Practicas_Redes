#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv){
    if (argc < 3){
        std::cerr << "Invalid arguments\n";
        return -1;
    }

    struct addrinfo info;
    struct addrinfo* res;

    //Reserva memoria para un addrinfo con valor 0 para el uso de info.
    memset((void*)&info, 0, sizeof(struct addrinfo));
    info.ai_flags = AI_PASSIVE;
    info.ai_family = AF_INET;
    info.ai_socktype = SOCK_STREAM;

    //Dado un host y un servicio, devolviendo la informacion en res y sus caracteristicas en info.
    int ret = getaddrinfo(argv[1], argv[2], &info, &res);
    if (ret != 0){
        std::cerr << "Error in getaddrinfo: " << gai_strerror(ret) << "\n";
        return -1;
    }

    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock == -1){
        std::cerr << "Error creating socket\n";
        return -1;
    }
    ret = bind(sock, res->ai_addr, res->ai_addrlen);
    if (ret == -1){
        std::cerr << "Error binding socket\n";
        return -1;
    }

    ret = listen(sock, 5);
    if (ret == -1){
        std::cerr << "Error binding socket\n";
        return -1;
    }

    struct sockaddr cliente;
    socklen_t clientelen = sizeof(struct sockaddr);
    int cliente_sd = accept(sock, &cliente, &clientelen);
    if (cliente_sd == -1){
        std::cerr << "Error accepting client\n";
        return -1;
    }

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    ret = getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV );
    if (ret != 0){
        std::cerr << "Error in getnameinfo\n";
        return -1;
    }
    std::cout << "Conexión desde " << host << " " << serv << "\n";

    int bytes;
    char buffer[80];
    while (true){
        bytes = recv(cliente_sd, (void*)buffer, 79, 0);
        if (bytes == -1){
            std::cerr << "Error sending message\n";
            return -1;
        }
        else if (bytes == 0) {
            std::cout << "Conexión terminada\n";
            ret = close(cliente_sd);
            if (ret == -1){
                std::cerr << "Error closing socket\n";
                return -1;
            }
            cliente_sd = accept(sock, &cliente, &clientelen);
            if (cliente_sd == -1){
                std::cerr << "Error accepting client\n";
                return -1;
            }
            ret = getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV );
            if (ret != 0){
                std::cerr << "Error in getnameinfo\n";
                return -1;
            }
            std::cout << "Conexión desde " << host << " " << serv << "\n";
        }

        buffer[bytes] = '\0';
        send(cliente_sd, buffer, bytes, 0);
    }
    freeaddrinfo(res);
    ret = close(sock);
    if (ret == -1){
        std::cerr << "Error closing socket\n";
        return -1;
    }
    return 0;
}