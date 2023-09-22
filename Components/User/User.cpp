#include "User.hpp"

/**************************************************************/
/*                        Constructors                        */
/**************************************************************/
User::User(int clientFd) : clientFd(clientFd)
{
    this->nickName = "";
    this->userName = "";
    this->hostName = "";
    this->serverName = "";
    this->realName = "";
    this->afk = false;
}

User::User(const User &user)
{
    if (this != &user)
    {
        this->clientFd = user.getClientFd();
        this->nickName = user.getNickName();
        this->userName = user.getUserName();
        this->hostName = user.getHostName();
        this->serverName = user.getServerName();
        this->realName = user.getRealName();
        this->afk = user.getAfk();
    }
}

/**************************************************************/
/*                    getters and setters                     */
/**************************************************************/
int User::getClientFd() const{ return this->clientFd;}
void User::setClientFd(int clientFd){ this->clientFd = clientFd;}
std::string User::getNickName() const{ return this->nickName;}
void User::setNickName(std::string nickName){ this->nickName = nickName;}
std::string User::getUserName() const{ return this->userName;}
void User::setUserName(std::string userName){ this->userName = userName;}
std::string User::getServerName() const{ return this->serverName;}
void User::setServerName(std::string serverName){ this->serverName = serverName;}
std::string User::getHostName() const{ return this->hostName;}
void User::setHostName(std::string hostName){ this->hostName = hostName;}
std::string User::getRealName() const{ return this->realName; }
void User::setRealName(std::string realName){ this->realName = realName;}
bool User::getIsConnected() const{return this->isConnected;}
void User::setIsConected(bool isConnected){ this->isConnected = isConnected; }
bool User::getAfk() const{return this->afk; }
void User::setAfk(bool afk){ this->afk = afk; }


/**************************************************************/
/*                         Functions                          */
/**************************************************************/

std::string User::getInfo() const
{ 
    return this->nickName + " " + \
        this->userName + " " + this->hostName + " " \
        + this->serverName + " " + this->realName;
}

// void User::joinChannel(Channel* channel)
// {
//     if(this->isInChannel(channel->getName()))
//         return;
//     this->channels.insert(std::make_pair(channel->getName(), channel));
//     channel->addUser(this);
//     (void) channel;
// }

// void User::leaveChannel(Channel* channel)
// {
//     if (!this->isInChannel(channel->getName()))
//         return;
//     this->channels.erase(channel->getName());
//     channel->removeUser(this->clientFd);
// }

// bool User::isInChannel(std::string channelName)
// {
//     (void) channelName;
//     return this->channels.find(channelName) != this->channels.end();
//     return false;
// }

User::~User()
{
    
}