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
                std::string message;
                message = ":irc.leet.com 332 " + this->users.find(clientFd)->second.getNickName() + " " +channelName + " : " + this->channels.find(channelName)->second.getTopic() + "\r\n";
                send(clientFd, message.c_str(), message.size(), 0);
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
            std::string message;
            message = ":" + this->users.find(clientFd)->second.getNickName() + " TOPIC " + channelName + " :" + this->channels.find(channelName)->second.getTopic() + "\r\n";
            this->channels.find(channelName)->second.broadcast(&(this->users.find(clientFd)->second), message, &(this->responses), true);
            send(clientFd, message.c_str(), message.size(), 0);              
        }
    }
}
