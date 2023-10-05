#include "../Components/Components.hpp"

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

    public:
        Bot(int port, std::string address, std::string password);
        ~Bot();
        void connectToServer();
};