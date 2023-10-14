#include "Server.hpp"

void Server::cmdLeave(int clientFd, std::string data)
{
    if (!this->users.find(clientFd)->second.getIsConnected())
        sendErrRep(451, clientFd, "PART", "", "");
    else if (data.empty())
        sendErrRep(461, clientFd, "PART", "", "");
    else
    {
        std::stringstream ss(data);
        std::string channels, channel, message;
        ss >> channels;
        ss >> std::ws;
        std::getline(ss, message);
        std::stringstream ss2(channels);
        while (std::getline(ss2, channel, ','))
        {
            if (channel.empty())
                continue;
            if (channel[0] != '#')
            {
                sendErrRep(403, clientFd, "PART", this->users.find(clientFd)->second.getNickName(), channel);
                continue;
            }
            if (this->channels.find(channel) == this->channels.end())
            {
                sendErrRep(403, clientFd, "PART", this->users.find(clientFd)->second.getNickName(), channel);
                continue;
            }
            if (!this->users.find(clientFd)->second.isInChannel(channel))
            {
                sendErrRep(442, clientFd, "PART", this->users.find(clientFd)->second.getNickName(), channel);
                continue;
            }
            this->users.find(clientFd)->second.leaveChannel(&(this->channels.find(channel)->second));
            std::string msg = ":" + this->users[clientFd].getNickName() + "!~" + this->users[clientFd].getUserName() + "@" + this->users[clientFd].getHostName() + " PART " + this->channels[channel].getName() + "\r\n";
            send(clientFd, msg.c_str(), msg.size(), 0);
            this->channels[channel].broadcast(&(this->users.find(clientFd)->second), "PART " + channel + " :" + message, NULL, true);
            if (this->channels.find(channel) != this->channels.end() && this->channels[channel].getMemberCount() == 0)
                this->channels.erase(channel);
        }
    }
}