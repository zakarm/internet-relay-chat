#ifndef SERVER_HPP
#define SERVER_HPP
#include "../Components.hpp"
#include "../../Utils/Utils.hpp"
#include "../Client/Client.hpp"
#include "../Channel/Channel.hpp"

class Server{
    private :
        int serverSocket;
        struct sockaddr_in serverAddr;
        int socketLen;
        unsigned int port;
        std::vector<struct pollfd> pfds;
        std::vector<Client> clients;
        std::vector<Channel> channels;
        std::queue<std::pair<int, std::string> > responses;

    public :
        Server(int port);
        Server(const Server& sv);
        int getServerSocket() const;
        void setServerSocket(int serverSocket);

        struct sockaddr_in getServerAddr() const;
        void setServerAddr(struct sockaddr_in serverAddr);

        int getSocketLen() const;
        void setSocketLen(int socketLen);

        unsigned int getPort() const;
        void setPort(unsigned int port);

        void customException(std::string errorMessage);
        void noBlockingFd();
        void socketOptions();
        void bindServer();
        void listenServer();
        void acceptClients();
        void requests(int indexClient);
        void runServer();

        void loginClient(size_t indexClient);
        void runCommand(size_t indexClient, std::string command);
};

#endif