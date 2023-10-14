#include "Server.hpp"

void Server::cmdTopic(int clientFd, std::string data)
{
    if (!this->users.find(clientFd)->second.getIsConnected())
        sendErrRep(451, clientFd, "TOPIC", "", "");
    else if (data.empty())
        sendErrRep(461, clientFd, "TOPIC", "", "");
    else
    {
        std::stringstream ss(data);
        std::string channelName, newTopic;
        ss >> channelName;
        ss >> std::ws;
        std::getline(ss, newTopic);
        if (this->channels.find(channelName) == this->channels.end())
            sendErrRep(403, clientFd, "TOPIC", this->users.find(clientFd)->second.getNickName(), channelName);
        else if (!this->users.find(clientFd)->second.isInChannel(channelName))
            sendErrRep(442, clientFd, "TOPIC", this->users.find(clientFd)->second.getNickName(), channelName);
        else if (this->channels.find(channelName) != this->channels.end() && newTopic.empty())
        {
            if (this->channels.find(channelName)->second.getTopic().empty())
                sendErrRep(331, clientFd, "TOPIC", this->users.find(clientFd)->second.getNickName(), channelName);
            else
            {
                sendErrRep(332, clientFd, "TOPIC", this->users.find(clientFd)->second.getNickName(), channelName + " " + \
                            this->channels.find(channelName)->second.getTopic());
                sendErrRep(333, clientFd, "TOPIC", this->users.find(clientFd)->second.getNickName(), channelName + " " + \
                            this->users.find(clientFd)->second.getNickName() + " " + Utils::getDate());
            }
        }
        else
        {
            if (this->channels.find(channelName)->second.getMode() & Channel::TOPIC_PROTECTED && !this->channels.find(channelName)->second.isOperator(clientFd))
            {
                sendErrRep(482, clientFd, "TOPIC", this->users.find(clientFd)->second.getNickName(), channelName);
                return;
            }
            if (!newTopic.empty() && newTopic[0] == ':' && newTopic.size() == 1)
                this->channels.find(channelName)->second.setTopic("");
            else if (newTopic[0] == ':')
                this->channels.find(channelName)->second.setTopic(newTopic.substr(1, newTopic.size()));
            else
                this->channels.find(channelName)->second.setTopic(newTopic);
            std::string clientNick(this->users[clientFd].getNickName());
            std::map<int, User *>::const_iterator it;
            for (it = this->channels[channelName].getUsers().begin(); it != this->channels[channelName].getUsers().end(); it++)
            {
                if (it->second->getIsConnected())
                    sendErrRep(332, it->second->getClientFd(), "", clientNick,
                               channelName + " :" + this->channels.find(channelName)->second.getTopic());
            }
            for (it = this->channels[channelName].getOperators().begin(); it != this->channels[channelName].getOperators().end(); it++)
            {
                if (it->second->getIsConnected())
                    sendErrRep(332, it->second->getClientFd(), "", clientNick,
                               channelName + " :" + this->channels.find(channelName)->second.getTopic());
            }
        }
    }
}