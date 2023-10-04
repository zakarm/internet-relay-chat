#include "User.hpp"

/**************************************************************/
/*                        Constructors                        */
/**************************************************************/

User::User()
{
    this->clientFd = -1;
    this->nickName = "";
    this->userName = "";
    this->hostName = "";
    this->serverName = "";
    this->realName = "";
    this->isConnected = false;
    this->setPass = false;
    this->afk = false;
}

User::User(int clientFd) : clientFd(clientFd)
{
    this->nickName = "";
    this->userName = "";
    this->hostName = "";
    this->serverName = "";
    this->realName = "";
    this->isConnected = false;
    this->setPass = false;
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
        this->isConnected = user.getIsConnected();
        this->setPass = user.getSetPass();
        this->afk = user.getAfk();
    }
}

/**************************************************************/
/*                    getters and setters                     */
/**************************************************************/
int User::getClientFd() const{ return this->clientFd; }
void User::setClientFd(int clientFd){ this->clientFd = clientFd; }
std::string User::getNickName() const{ return this->nickName; }
void User::setNickName(std::string nickName){ this->nickName = nickName; }
std::string User::getUserName() const{ return this->userName; }
void User::setUserName(std::string userName){ this->userName = userName; }
std::string User::getServerName() const{ return this->serverName; }
void User::setServerName(std::string serverName){ this->serverName = serverName; }
std::string User::getHostName() const{ return this->hostName; }
void User::setHostName(std::string hostName){ this->hostName = hostName; }
std::string User::getRealName() const{ return this->realName; }
void User::setRealName(std::string realName){ this->realName = realName; }
bool User::getIsConnected() const{return this->isConnected; }
void User::setIsConected(bool isConnected){ this->isConnected = isConnected; }
bool User::getSetPass() const{ return this->setPass; }
void User::setSetPass(bool setPass){ this->setPass = setPass; }
bool User::getAfk() const{return this->afk; }
void User::setAfk(bool afk){ this->afk = afk; }
std::string User::getInfo() const
{ 
    return this->nickName + " " + \
        this->userName + " " + this->hostName + " " \
        + this->serverName + " " + this->realName;
}


/**************************************************************/
/*                         Functions                          */
/**************************************************************/


void User::joinChannel(Channel* channel)
{
    if(this->isInChannel(channel->getName()))
        return; // send already in channel.
    this->channels.insert(std::make_pair(channel->getName(), channel));
    channel->addUser(this);
}

void User::leaveChannel(Channel* channel) 
{
    if(!this->isInChannel(channel->getName()))
        return; // send not in channel.
    this->channels.erase(channel->getName());
    channel->removeUser(this->clientFd);
}

bool User::isInChannel(std::string channelName)
{
    return this->channels.find(channelName) != this->channels.end();
}

void User::addChannel(Channel* channel)
{
    if(this->isInChannel(channel->getName()))
        return; // send already in channel.
    this->channels.insert(std::make_pair(channel->getName(), channel));
}


void User::leaveAllChannels(std::queue<std::pair<int, std::string> > *queue)
{
    std::map<std::string, Channel*>::iterator it = this->channels.begin();
    while (it != this->channels.end())
    {
        it->second->broadcast(this, "PART " + it->second->getName(), queue);
        it->second->removeUser(this->clientFd);
        it++;
    }
    this->channels.clear();
}

User::~User()
{
    std::map<std::string, Channel*>::iterator it = this->channels.begin();
    while (it != this->channels.end())
    {
        it->second->removeUser(this->clientFd);
        it++;
    }
}