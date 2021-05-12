#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv){
    if (argc < 4){
        std::cerr << "Invalid arguments\n";
        return -1;
    }

    struct addrinfo info;
    struct addrinfo* res1;

    //Reserva memoria para un addrinfo con valor 0 para el uso de info.
    memset((void*)&info, 0, sizeof(struct addrinfo));
    info.ai_family = AF_INET;
    info.ai_socktype = SOCK_DGRAM;

    //Dado un host y un servicio, devolviendo la informacion en res y sus caracteristicas en info.
    int ret = getaddrinfo(argv[1], "9999", &info, &res1);
    if (ret != 0){
        std::cerr << "Error in getaddrinfo: " << gai_strerror(ret) << "\n";
        return -1;
    }

    int sock = socket(res1->ai_family, res1->ai_socktype, 0);
    if (sock == -1){
        std::cerr << "socket error: " << strerror(errno) << "\n";
        return -1;
    }
    ret = bind(sock, res1->ai_addr, res1->ai_addrlen);
    if (ret == -1){
        std::cerr << "bind error: " << strerror(errno) << "\n";
        return -1;
    }
    freeaddrinfo(res1);

    struct addrinfo* res2;

    ret = getaddrinfo(argv[1], argv[2], &info, &res2);
    if (ret == -1){
        std::cerr << "Error in getaddrinfo: " << gai_strerror(ret) << "\n";
        return -1;
    }
    ret = sendto(sock, argv[3], sizeof(char), 0, res2->ai_addr, res2->ai_addrlen);
    if (ret == -1){
        std::cerr << "sendto error: " << strerror(errno) << "\n";
        return -1;
    }
    char buffer[80];
    int bytes = recvfrom(sock, buffer, 80, 0, res2->ai_addr, &res2->ai_addrlen);
    if (bytes == -1){
        std::cerr << "recvfrom error: " << strerror(errno) << "\n";
        return -1;
    }
    buffer[bytes] = '\0';
    std::cout << buffer << "\n";
    freeaddrinfo(res2);
    ret = close(sock);
    if (ret == -1){
        std::cerr << "close error: " << strerror(errno) << "\n";
        return -1;
    }
    return 0;
}