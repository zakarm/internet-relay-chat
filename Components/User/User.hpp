
#pragma once
#include "../Components.hpp"

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
        bool afk;
        std::vector<Channel&> channels;
        
    public :
        User(int clientFd);
        User(const User &user);
        ~User();

 #pragma region 
        std::string getNickName() const;
        std::string getUserName() const;
        std::string getHostName() const;
        std::string getServerName() const;
        std::string getRealName() const;
        std::string getInfo() const;

        void setNickName(std::string nickName);
        void setUserName(std::string userName);
        void setHostName(std::string hostName);
        void setServerName(std::string serverName);
        void setRealName(std::string realName);
#pragma endregion

        void joinChannel(std::string channelName);
        void leaveChannel(std::string channelName);
        void setAfk(bool afk);
}
