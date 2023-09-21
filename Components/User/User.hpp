
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
        std::map<std::string, Channel*> channels;
        
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
        std::map<std::string, Channel*> getChannels() const{ return this->channels;}

        void setNickName(std::string nickName){ this->nickName = nickName;}
        void setUserName(std::string userName){ this->userName = userName;}
        void setHostName(std::string hostName){ this->hostName = hostName;}
        void setServerName(std::string serverName){ this->serverName = serverName;}
        void setRealName(std::string realName){ this->realName = realName;}
        void setClientFd(int clientFd){ this->clientFd = clientFd;}
        void setAfk(bool afk){ this->afk = afk;}


        void sendMessage(std::string message);
        void joinChannel(Channel* channel);
        void leaveChannel(Channel* channel);
        bool isInChannel(std::string channelName);
};

#endif