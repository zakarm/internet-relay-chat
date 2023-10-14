#include "Server.hpp"

void Server::cmdPrivMsg(int clientFd, std::string data)
{
    std::string prefix("PRIVMSG ");
    if (!this->users.find(clientFd)->second.getIsConnected())
        sendErrRep(451, clientFd, "PRIVMSG", "", "");
    else
    {
        std::stringstream ss(data);
        std::string targets, message, target;

        ss >> targets;
        ss >> std::ws;
        std::getline(ss, message);

        if (targets.empty())
        {
            sendErrRep(411, clientFd, "PRIVMSG", "", "");
            return;
        }
        if (message.empty())
        {
            sendErrRep(412, clientFd, "PRIVMSG", this->errRep.find(412)->second, "");
            return;
        }
        if (message[0] == ':')
            message = message.substr(1, message.length() - 1);
        std::stringstream ss2(targets);
        while (std::getline(ss2, target, ','))
        {
            if (target.empty())
                continue;
            if (target[0] == '#' || target[1] == '#')
            {
                int to;
                to = target[0] == '%' ?  1 : 0;
                target = target.substr(to, target.length());
                if (!this->users[clientFd].isInChannel(target))
                {
                    sendErrRep(442, clientFd, "PRIVMSG", this->users.find(clientFd)->second.getNickName(), target); 
                    continue;
                }
                message = target + " :" + message;
                if (this->channels.find(target) != this->channels.end())
                    this->channels[target].broadcast(&(this->users.find(clientFd)->second),prefix + message, &(this->responses), true);
                else
                    sendErrRep(401, clientFd, "PRIVMSG", this->users.find(clientFd)->second.getNickName(), target);
            }
            else
            {

                if (this->nicks.find(target) == this->nicks.end())
                    sendErrRep(401, clientFd, "PRIVMSG", this->users.find(clientFd)->second.getNickName(), target);
                else
                {
                    std::string msg;
                    msg = ":" + this->users[clientFd].getNickName()+ "!" + this->users[clientFd].getUserName() + "@" + this->users[clientFd].getHostName() + " PRIVMSG " + target + " :" + message + "\r\n";
                    send(this->nicks[target], msg.c_str(), msg.size(), 0);
                }
            }
        }
    }
}

void Server::cmdNotice(int clientFd, std::string data)
{
    std::string prefix("NOTICE ");
    if (!this->users.find(clientFd)->second.getIsConnected())
        return;
    std::stringstream ss(data);
    std::string targets, message, target;
    ss >> targets;
    ss >> std::ws;
    std::getline(ss, message);
    if (targets.empty())
        return;
    if (message.empty())
        return;
    if (message[0] == ':')
        message = message.substr(1, message.length() - 1);
    std::stringstream ss2(targets);
    while (std::getline(ss2, target, ','))
    {
        if (target.empty())
            continue;
        if (target[0] == '#' || target[1] == '#')
        {
            int to;
            to = target[0] == '%' ?  1 : 0;
            target = target.substr(to, target.length());
            if (!this->users[clientFd].isInChannel(target))
                continue;
            message = target + " :" + message;
            if (this->channels.find(target) != this->channels.end())
                this->channels[target].broadcast(&(this->users.find(clientFd)->second),prefix + message, &(this->responses), true);
        }
        else
        {

            if (this->nicks.find(target) == this->nicks.end())
                continue;
            std::string msg;
            msg = ":" + this->users[clientFd].getNickName()+ "!" + this->users[clientFd].getUserName() + "@" + this->users[clientFd].getHostName() + " NOTICE " + target + " :" + message + "\r\n";
            send(this->nicks[target], msg.c_str(), msg.size(), 0);
        }
    }
}