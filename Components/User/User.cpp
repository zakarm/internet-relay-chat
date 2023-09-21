

#include "User.hpp"

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


void User::joinChannel(Channel* channel)
{
    if(this->isInChannel(channel->getName()))
        return;
    //banlist
    this->channels.insert(std::make_pair(channel->getName(), channel));
    channel->addUser(*this);
}

void User::leaveChannel(Channel* channel)
{
    if (!this->isInChannel(channel->getName()))
        return;
    this->channels.erase(channel->getName());
    channel->removeUser(this->clientFd);
}

bool User::isInChannel(std::string channelName)
{
    return this->channels.find(channelName) != this->channels.end();
}

User::~User()
{
    for(std::map<std::string, Channel*>::iterator it = this->channels.begin(); it != this->channels.end(); it++)
        it->second->removeUser(this->clientFd);
}