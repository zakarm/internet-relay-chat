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

        std::queue<std::pair<int, t_message> > responses;
        std::map<int, std::string> buffring;
        std::map<int, User> users;
        std::map<std::string, Channel> channels;
        std::map<std::string, int> nicks;
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
        int getUserFdByNick(std::string nick) const;


        void customException(std::string errorMessage);
        void noBlockingFd();
        void socketOptions();
        void bindServer();
        void listenServer();
        void acceptAndDecline();
        void multipleClients();
        void clientDisconnected(int clientFd);
        void requests(int indexClient);
        void runServer();

        void sendErrRep(int code, int clientFd, std::string command, std::string s1, std::string s2);
        bool checkPass(std::string password);
        bool checkDuplicateNick(std::string nickName);
        int  getUserFdByNick(std::string nick);
        bool validNick(const std::string& data);
        void cmdPass(int clientFd, std::string data);
        void cmdNick(int clientFd, std::string data);
        void cmdTopic(int clientFd, std::string data);
        void cmdInvite(int clientFd, std::string data);
        void cmdKick(int clientFd, std::string data);
        void cmdJoin(int clientFd, std::string data);
        void cmdUser(int clientFd, std::string data);
        void cmdLeave(int clientFd, std::string data);
        void cmdBot(int clientFd, std::string data);
        void cmdNotice(int clientFd, std::string data);
        void authenticate(int clientFd);
        void runCommand(int clientFd, std::string command);
        int  userCheck(std::string data, int ft_clientFd);
        bool checkDuplicateUser(int clientFd);
        void cmdPrivMsg(int clientFd, std::string data);
        void cmdMode(int clientFd, std::string cmd);
        void i_mode(std::string channel, char sign, int clientFd);
        // void l_mode(int clientFd, std::string cmd);
        void l_mode(int clientFd, char sign , std::string channel, std::stringstream &ss);
        // void o_mode(int clientFd, std::string cmd);
        void o_mode (int clientFd, char sign , std::string channel, std::stringstream &ss);
        std::string set_operator(std::string channel, User *user, std::string mode);
        void t_mode(std::string channel, char sign, int clientFd);
        // void k_mode(std::string cmd, int clientFd);
        void k_mode(int clientFd, char sign , std::string channel, std::stringstream &ss);
        void cmdAuthBot(int clientFd, std::string command);
        int  count(std::string str);
        void printModemessage(std::string channel, std::string mode, int clientFd);
        void emptyMode(std::string channel, int clientFd);
};
void   port_check(const char *port);
#endif