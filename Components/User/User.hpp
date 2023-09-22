
#ifndef USER_HPP
#define USER_HPP
# include "../Components.hpp"
# include "../Channel/Channel.hpp"

class Channel;

class User
{
    private :
        int clientFd;
        std::string nickName;
        std::string userName;
        std::string hostName;
        std::string serverName;
        std::string realName;
        bool isConnected;
        bool afk;
        
    public :
        User(int clientFd);
        User(const User &user);
        ~User();

        int getClientFd() const;
        void setClientFd(int clientFd);
        std::string getNickName() const;
        void setNickName(std::string nickName);
        std::string getUserName() const;
        void setUserName(std::string userName);
        std::string getHostName() const;
        void setHostName(std::string hostName);
        std::string getServerName() const;
        void setServerName(std::string serverName);
        std::string getRealName() const;
        void setRealName(std::string realName);
        bool getIsConnected() const;
        void setIsConected(bool isConnected);
        bool getAfk() const;
        void setAfk(bool afk);

        std::string getInfo() const;
        void sendMessage(std::string message);
        void joinChannel(Channel* channel);
        void leaveChannel(Channel* channel);
        bool isInChannel(std::string channelName);
};

#endif