#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

int main(int argc, char** argv){
    if (argc > 5 || argc < 3){
        std::cerr << "Invalid arguments\n";
        return -1;
    }
    int num = 80;
    else if (argc == 4) num = argv[3];

    struct addrinfo info;
    struct addrinfo* res;

    //Reserva memoria para un addrinfo con valor 0 para el uso de info.
    memset((void*)&info, 0, sizeof(struct addrinfo));
    info.ai_family = AF_UNSPEC;

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
    bind(sock, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    while (true){
        char buffer[num];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockadrr cliente;
        socklen_t clientelen = sizeof(struct sockadrr);
        int bytes = recvfrom(sock, (void*)buffer, num, 0, &cliente, &clientelen);

        if (bytes == -1){
            std::cerr << "Cannot receive bytes from address\n";
            return -1;
        }

        ret = getnameinfo(&cliente, &clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        if (ret != 0){
            std::cerr << "Error in getnameinfo\n";
            return -1;
        }

        std::cout << "Host: " << host << " Port: " << serv << "\n";
        std::cout << "\tData: " << buffer << "\n";

        ret = sendto(sock, buffer, bytes, 0, &cliente, &clientelen);
        if (ret != 0){
            std::cerr << "Error in sendto\n";
            return -1;
        }
    }

    return 0;
}