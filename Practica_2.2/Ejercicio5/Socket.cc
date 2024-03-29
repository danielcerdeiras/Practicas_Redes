#include <string.h>

#include "Serializable.h"
#include "Socket.h"

Socket::Socket(const char * address, const char * port):sd(-1)
{
    //Construir un socket de tipo AF_INET y SOCK_DGRAM usando getaddrinfo.
    //Con el resultado inicializar los miembros sd, sa y sa_len de la clase

    struct addrinfo info;
    struct addrinfo* res;

    //Reserva memoria para un addrinfo con valor 0 para el uso de info.
    memset((void*)&info, 0, sizeof(struct addrinfo));
    info.ai_family = AF_INET;
    info.ai_socktype = SOCK_DGRAM;

    int ret = getaddrinfo(address, port, &info, &res);
    if (ret != 0){
        std::cerr << "Error in getaddrinfo: " << gai_strerror(ret) << "\n";
        return;
    }

    sd = socket(res->ai_family, res->ai_socktype, 0);
    if (sd == -1){
        std::cerr << "socket error: " << strerror(errno) << "\n";
        return;
    }

    sa = *res->ai_addr;
    sa_len = res->ai_addrlen;

    freeaddrinfo(res);

    bind();
}

int Socket::recv(Serializable &obj, Socket * &sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];

    ssize_t bytes = ::recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    if ( bytes <= 0 )
    {
        return -1;
    }

    if ( sock != 0 )
    {
        sock = new Socket(&sa, sa_len);
    }

    obj.from_bin(buffer);

    return 0;
}

int Socket::send(Serializable& obj, const Socket& sock)
{
    //Serializar el objeto
    //Enviar el objeto binario a sock usando el socket sd

    obj.to_bin();
    int ret = sendto(sd, obj.data(), MAX_MESSAGE_SIZE, 0, &sock.sa, sock.sa_len);
    if (ret == -1) {
        std::cerr << "sendto error: " << strerror(errno) << "\n";
        return -1;
    }
    return 0;
}

bool operator== (const Socket &s1, const Socket &s2)
{
    //Comparar los campos sin_family, sin_addr.s_addr y sin_port
    //de la estructura sockaddr_in de los Sockets s1 y s2
    //Retornar false si alguno difiere

    struct sockaddr_in* addr1 = (struct sockaddr_in*) &s1.sa;
    struct sockaddr_in* addr2 = (struct sockaddr_in*) &s2.sa;

    return (addr1->sin_family == addr2->sin_family &&
            addr1->sin_addr.s_addr == addr2->sin_addr.s_addr &&
            addr1->sin_port == addr2->sin_port);
    return true;
};

std::ostream& operator<<(std::ostream& os, const Socket& s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    os << host << ":" << serv;

    return os;
};

