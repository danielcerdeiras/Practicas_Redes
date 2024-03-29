#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    char* tmp = _data;

    memcpy(tmp, &type, sizeof(uint8_t));
    tmp += sizeof(uint8_t);

    memcpy(tmp, nick.c_str(), 8 * sizeof(char));
    tmp += 8 * sizeof(char);

    memcpy(tmp, message.c_str(), 80 * sizeof(char));
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    char* tmp = _data;

    memcpy(&type, tmp, sizeof(uint8_t));
    tmp += sizeof(uint8_t);

    nick = tmp;
    tmp += 8 * sizeof(char);

    message = tmp;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        ChatMessage mess;
        Socket* client;
        socket.recv(mess, client);
        std::cout << "Type: " << int(mess.type) << " Nick: " << mess.nick << " Message: " << mess.message << "\n";
        switch (mess.type)
        {
        case ChatMessage::LOGIN:
            clients.push_back(std::move(std::unique_ptr<Socket>(client)));
            break;
        case ChatMessage::LOGOUT:
            {
            auto it = clients.begin();
            while (it != clients.end()){
                if (*it->get() == *client) {
                    clients.erase(it);
                    it->release();
                    break;
                }
                ++it;
            }
            break;
            }
        case ChatMessage::MESSAGE:
            {
            auto it = clients.begin();
            while (it != clients.end()){
                if (!(*it->get() == *client))
                    socket.send(mess, *it->get());
                ++it;
            }
            break;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    if (socket.send(em, socket) == -1){
        std::cerr << "send error: " << strerror(errno) << "\n";
    }
}

void ChatClient::logout()
{

}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}