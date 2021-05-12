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
    info.ai_socktype = SOCK_DGRAM;

    //Dado un host y un servicio, devolviendo la informacion en res y sus caracteristicas en info.
    int ret = getaddrinfo(argv[1], argv[2], &info, &res);
    if (ret != 0){
        std::cerr << "Error in getaddrinfo: " << gai_strerror(ret) << "\n";
        return -1;
    }

    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock == -1){
        std::cerr << "socket error: " << strerror(errno) << "\n";
        return -1;
    }
    ret = bind(sock, res->ai_addr, res->ai_addrlen);
    if (ret == -1){
        std::cerr << "bind error: " << strerror(errno) << "\n";
        return -1;
    }

    int bytes;
    char buffer[1]; buffer[0] = ' ';
    char timeBuffer[80];
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    struct sockaddr cliente;
    socklen_t clientelen = sizeof(struct sockaddr);
    while (buffer[0] != 'q'){
        bytes = recvfrom(sock, (void*)buffer, 80, 0, &cliente, &clientelen);
        if (bytes == -1){
            std::cerr << "recvfrom error: " << strerror(errno) << "\n";
            return -1;
        }

        ret = 0;
        
        if (buffer[0] == 'd') {
            time_t rawTime;
            struct tm* timeInfo;
            time(&rawTime);
            timeInfo = localtime(&rawTime);
            int timeSize = strftime(timeBuffer, 80, "%Y-%m-%d", timeInfo);
            ret = sendto(sock, timeBuffer, timeSize, 0, &cliente, clientelen);
        }
        else if (buffer[0] == 't') {
            time_t rawTime;
            struct tm * timeInfo;
            time(&rawTime);
            timeInfo = localtime(&rawTime);
            int timeSize = strftime(timeBuffer, 80, "%I:%M:%S %p", timeInfo);
            ret = sendto(sock, timeBuffer, timeSize, 0, &cliente, clientelen);
        }
        else if (buffer[0] == 'q') std::cout << "Saliendo...\n";
        else std::cout << "Comando no soportado " << buffer[0] << "\n";
        if (ret == -1){
            std::cerr << "socket error: " << strerror(errno) << "\n";
            return -1;
        }

        if (buffer[0] != 'q') {
            ret = getnameinfo(&cliente, clientelen,host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV );
            if (ret != 0){
                std::cerr << "getnameinfo error: " << strerror(errno) << "\n";
                return -1;
            }  
            std::cout << bytes << " bytes de " << host << ":" << serv << "\n";
        }
    }
    freeaddrinfo(res);
    ret = close(sock);
    if (ret == -1){
        std::cerr << "close error: " << strerror(errno) << "\n";
        return -1;
    }
    return 0;
}