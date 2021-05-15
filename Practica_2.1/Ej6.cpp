#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <thread>

#define MAX_THREADS 3

class MessageThread {
    static const int BUFFSIZE = 80;
public:
    MessageThread(int socket) : _socket(socket) {
    }
    void do_message() {
        struct sockaddr _cliente;
        socklen_t _clientelen = sizeof(struct sockaddr);
        while(true){
            int bytes = recvfrom(_socket, (void*)_input, 1, 0, &_cliente, &_clientelen);
            if (bytes == -1){
                std::cerr << "recvfrom error: " << strerror(errno) << "\n";
                return;
            }
            sleep(3);
            int ret;
            time_t rawTime;
            struct tm* timeInfo;
            time(&rawTime);
            timeInfo = localtime(&rawTime);
            if (_input[0] == 'd') {
                int timeSize = strftime(_buffer, 80, "%Y-%m-%d", timeInfo);
                ret = sendto(_socket, _buffer, BUFFSIZE, 0, &_cliente, _clientelen);
            }
            else if (_input[0] == 't') {
                int timeSize = strftime(_buffer, 80, "%I:%M:%S %p", timeInfo);
                ret = sendto(_socket, _buffer, BUFFSIZE, 0, &_cliente, _clientelen);
            }
            else std::cout << "Comando no soportado " << _input[0] << "\n";
            if (ret == -1){
                std::cerr << "socket error: " << strerror(errno) << "\n";
                return;
            }
            ret = getnameinfo(&_cliente, _clientelen, _host, NI_MAXHOST, _serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV );
            if (ret != 0){
                std::cerr << "getnameinfo error: " << strerror(errno) << "\n";
                return;
            }  
            std::cout << "Thread: " << std::this_thread::get_id() << ". " << bytes << " bytes de " << _host << ":" << _serv << "\n";
        }
    }
private:
    int _socket;
    char _input[1];
    char _buffer[BUFFSIZE];
    char _host[NI_MAXHOST];
    char _serv[NI_MAXSERV];
};

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

    for (int i = 0; i < MAX_THREADS; i++){
        MessageThread *mt = new MessageThread(sock);

        std::thread([mt](){
            mt->do_message();
            delete mt;
        }).detach();
    }
    char exit;
    while (exit != 'q'){
        std::cin >> exit;
    }

    freeaddrinfo(res);
    ret = close(sock);
    if (ret == -1){
        std::cerr << "close error: " << strerror(errno) << "\n";
        return -1;
    }
    return 0;
}