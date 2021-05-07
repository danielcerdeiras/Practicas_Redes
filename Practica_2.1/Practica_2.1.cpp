#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

int main(int argc, char** argv){
    if (argc > 3){
        std::cerr << "Invalid arguments\n";
        return -1;
    }

    struct addrinfo info;
    struct addrinfo* res;

    //Reserva memoria para un addrinfo con valor 0 para el uso de info.
    memset((void*)&info, 0, sizeof(struct addrinfo));
    info.ai_family = AF_UNSPEC;
    info.ai_socktype = SOCK_STREAM;

    //Dado un host y un servicio, devolviendo la informacion en res y sus caracteristicas en info.
    int ret = getaddrinfo(argv[1], argv[2], &info, &res);
    if (ret != 0){
        std::cerr << "Error in getaddrinfo: " << gai_strerror(ret) << "\n";
        return -1;
    }

    for (auto i = res; i != nullptr; i = i->ai_next){
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        ret = getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST);
        if (ret != 0){
        std::cerr << "Error in getnameinfo\n";
        return -1;
    }
        std::cout << "Host: " << host << " Port: " << serv << " Family: " << i->ai_family << " Socket Type: " << i->ai_socktype << "\n";
    }
    freeaddrinfo(res);

    return 0;
}