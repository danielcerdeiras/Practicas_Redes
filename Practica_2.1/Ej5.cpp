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
    info.ai_family = AF_INET;
    info.ai_socktype = SOCK_STREAM;

    //Dado un host y un servicio, devolviendo la informacion en res y sus caracteristicas en info.
    int ret = getaddrinfo(argv[1], "9999", &info, &res);
    if (ret != 0){
        std::cerr << "Error in getaddrinfo: " << gai_strerror(ret) << "\n";
        return -1;
    }

    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock == -1){
        std::cerr << "socket error: " << strerror(errno) << "\n";
        return -1;
    }

    ret = getaddrinfo(argv[1], argv[2], &info, &res);
    if (ret != 0){
        std::cerr << "Error in getaddrinfo: " << gai_strerror(ret) << "\n";
        return -1;
    }

    ret = connect(sock, res->ai_addr, res->ai_addrlen);
    if (ret == -1){
        std::cerr << "connect error: " << strerror(errno) << "\n";
        return -1;
    }

    int bytes;
    char buffer[80];
    while (true){
        std::cin >> buffer;
        if (buffer[0] == 'Q' && buffer[1] == '\0') break;
        ret = send(sock, buffer, strlen(buffer), 0);
        if (ret == -1){
            std::cerr << "send error: " << strerror(errno) << "\n";
            return -1;
        }
        bytes = recv(sock, (void*)buffer, 79, 0);
        if (bytes == -1){
            std::cerr << "recv error: " << strerror(errno) << "\n";
            return -1;
        }
        std::cout << buffer << "\n";
    }
    freeaddrinfo(res);
    ret = close(sock);
    if (ret == -1){
        std::cerr << "close error: " << strerror(errno) << "\n";
        return -1;
    }
    return 0;
}