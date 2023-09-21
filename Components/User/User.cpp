

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



User::~User()
{
}