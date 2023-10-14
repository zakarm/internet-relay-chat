#include "Server.hpp"

void Server::authenticate(int clientFd)
{
    if (!this->users.find(clientFd)->second.getIsConnected())
    {
        User u = this->users.find(clientFd)->second;
        if (!u.getUserName().empty() && !u.getNickName().empty() && u.getSetPass())
        {
            for (int i = 1; i <= 5; i++)
                sendErrRep(i, clientFd, "", "", "");
            this->users.find(clientFd)->second.setIsConected(true);
        }
    }
}

void Server::cmdPass(int clientFd, std::string data)
{

    std::stringstream err;
    if (data.empty())
        sendErrRep(461, clientFd, "PASS", "", "");
    else if (this->users.find(clientFd)->second.getIsConnected())
        sendErrRep(462, clientFd, "PASS", "", "");
    else if (!checkPass(data))
        sendErrRep(464, clientFd, "PASS", "", "");
    else
        this->users.find(clientFd)->second.setSetPass(true);
}