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
        // int getClientSocket() const;
        // void setClientSocket(int clientSocket);
        // struct sockaddr_in getClientAddr() const;
        // void setClientAddr(struct sockaddr_in clientAddr);
        // int getSocketLen() const;
        // void setSocketLen(int socketLen);
        // unsigned int getPort() const;
        // void setPort(unsigned int port);
        // std::string getPassword() const;
        // void setPassword(std::string password);
        // std::string getNickName() const;
        // void setNickName(std::string nickName);
        // std::string getUserName() const;
        // void setUserName(std::string userName);
        // std::string getHostName() const;
        // void setHostName(std::string hostName);
        // std::string getRealName() const;
        // void setRealName(std::string realName);

        void connectToServer();

};