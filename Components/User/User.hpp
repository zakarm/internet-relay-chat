
#pragma once
#include "../Components.hpp"

// class Channel;

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
        // std::vector<Channel&> channels;
        
    public :
        User(int clientFd);
        User(const User &user);
        ~User();

        std::string getNickName() const{ return this->nickName;}
        std::string getUserName() const{ return this->userName;}
        std::string getHostName() const{ return this->hostName;}
        std::string getServerName() const{ return this->serverName;}
        std::string getRealName() const{ return this->realName;}
        std::string getInfo() const{ return this->nickName + " " + \
        this->userName + " " + this->hostName + " " \
        + this->serverName + " " + this->realName;}
        int getClientFd() const{ return this->clientFd;}
        bool getAfk() const{ return this->afk;}
        // std::vector<Channel&> getChannels() const{ return this->channels;}

        void setNickName(std::string nickName){ this->nickName = nickName;}
        void setUserName(std::string userName){ this->userName = userName;}
        void setHostName(std::string hostName){ this->hostName = hostName;}
        void setServerName(std::string serverName){ this->serverName = serverName;}
        void setRealName(std::string realName){ this->realName = realName;}
        void setClientFd(int clientFd){ this->clientFd = clientFd;}
        void setAfk(bool afk){ this->afk = afk;}
        // void setChannels(std::vector<Channel&> channels){ this->channels = channels;}



        void sendMessage(std::string message);
        void joinChannel(std::string channelName);
        void leaveChannel(std::string channelName);
};