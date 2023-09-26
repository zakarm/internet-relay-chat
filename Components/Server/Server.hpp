#ifndef SERVER_HPP
#define SERVER_HPP
#include "../Components.hpp"
#include "../../Utils/Utils.hpp"
#include "../Channel/Channel.hpp"
#include "../User/User.hpp"

class Server{
    private :
        int serverSocket;
        struct sockaddr_in serverAddr;
        int socketLen;
        unsigned int port;
        std::string password;
        std::vector<struct pollfd> pfds;

        std::queue<std::pair<int, std::string> > responses;
        std::map<int, std::string> buffring;
        std::map<int, User> users;
        std::map<std::string, Channel> channels;
        std::map<int, std::string> errRep;

    public :
        Server(int port, std::string password);
        Server(const Server& sv);
        ~Server();
        int getServerSocket() const;
        void setServerSocket(int serverSocket);
        struct sockaddr_in getServerAddr() const;
        void setServerAddr(struct sockaddr_in serverAddr);
        int getSocketLen() const;
        void setSocketLen(int socketLen);
        unsigned int getPort() const;
        void setPort(unsigned int port);
        std::string getPassword() const;
        void setPassword(std::string password);

        void customException(std::string errorMessage);
        void noBlockingFd();
        void socketOptions();
        void bindServer();
        void listenServer();
        void acceptAndDecline();
        void multipleClients();
        void clientDisconnected(int clientFd);
        std::string joinBuffers(int indexClient, char *buffer);
        void requests(int indexClient);
        void runServer();

        void sendErrRep(int code, int clientFd, std::string command, std::string s1, std::string s2);
        bool checkPass(std::string password);
        bool checkDuplicateNick(std::string nickName);
        int  getClientFdbyNick(std::string nick);
        bool validNick(const std::string& data);
        // void err_431(int clientFd);
        // void err_432(int clientFd);
        // void err_433(int clientFd);
        void cmdPass(int clientFd, std::string data);
        void cmdNick(int clientFd, std::string data);
        void cmdUser(int clientFd, std::string data);
        void cmdTopic(int clientFd, std::string data);
        void cmdInvite(int clientFd, std::string data);
        void authenticate(int clientFd);
        void runCommand(int clientFd, std::string command);
};

#endif