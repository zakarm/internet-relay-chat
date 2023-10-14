#include "Server.hpp"

void Server::cmdNick(int clientFd, std::string data)
{
    if (!this->users[clientFd].getSetPass())
    {
        sendErrRep(451, clientFd, "NICK", "", "");
        return;
    }
    std::stringstream ss(data);
    std::string nickName;
    ss >> nickName;
    if (nickName.empty() || nickName == ":")
        sendErrRep(431, clientFd, "NICK", "", "");
    else if (!validNick(nickName) )
        sendErrRep(432, clientFd, "NICK", "", "");
    else if (!checkDuplicateNick(nickName) || nickName == "bot")
        sendErrRep(433, clientFd, "NICK", "", "");
    else
    {
        if (!this->users.find(clientFd)->second.getNickName().empty())
        {
            std::string oldNick = this->users.find(clientFd)->second.getNickName();
            this->nicks.erase(oldNick);
            std::string msg = ":" + oldNick + "!" + this->users.find(clientFd)->second.getUserName() + "@" + this->users.find(clientFd)->second.getHostName() + " NICK " + nickName +  "\r\n";
            send(clientFd, msg.c_str(), msg.size(), 0);
            for (std::map<int, User>::iterator it = this->users.begin(); it != this->users.end(); it++)
            {
                if (it->first != clientFd && it->second.getIsConnected())
                    send(it->first, msg.c_str(), msg.size(), 0);
            }
        }
        this->users.find(clientFd)->second.setNickName(nickName);
        authenticate(clientFd);
        this->nicks.insert(std::make_pair(nickName, clientFd));
    }
}