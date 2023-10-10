#ifndef BOT_HPP
#define BOT_HPP
#include "../Components/Components.hpp"
#include "../Utils/Utils.hpp"
class Bot
{
    private :
        int clientSocket;
        struct sockaddr_in clientAddr;
        int socketLen;
        unsigned int port;
        std::string password;
        std::string nickName;
        std::string userName;
        std::string realName;
        std::string serverName;
        std::string lastSenderNick;

    public:
        Bot(int port, std::string address, std::string password);
        ~Bot();
        void connectToServer();
        void runCommand(std::string data);
};
#endif