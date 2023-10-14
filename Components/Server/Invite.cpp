#include "Server.hpp"

void Server::cmdInvite(int clientFd, std::string data)
{
    if (!this->users.find(clientFd)->second.getIsConnected())
        sendErrRep(451, clientFd, "INVITE", "", "");
    else if (data.empty())
        sendErrRep(461, clientFd, "INVITE", "", "");
    else
    {
        std::stringstream ss(data);
        std::string nickname, channelName;
        ss >> nickname;
        ss >> std::ws;
        std::getline(ss, channelName);
        if (nickname.empty() || channelName.empty())
            sendErrRep(461, clientFd, "INVITE", "", "");
        else if (this->channels.find(channelName) == this->channels.end())
            sendErrRep(403, clientFd, "INVITE", this->users.find(clientFd)->second.getNickName(), channelName);
        else if (!this->users.find(clientFd)->second.isInChannel(channelName))
            sendErrRep(442, clientFd, "INVITE", this->users.find(clientFd)->second.getNickName(), channelName);
        else if (this->channels.find(channelName)->second.getMode() & Channel::INVITE_ONLY && !this->channels.find(channelName)->second.isOperator(clientFd))
            sendErrRep(482, clientFd, "INVITE", this->users.find(clientFd)->second.getNickName(), channelName);
        else
        {
            int target = getUserFdByNick(nickname);
            if (target > 0 && this->users.find(target)->second.isInChannel(channelName))
                sendErrRep(443, clientFd, "INVITE", this->users.find(clientFd)->second.getNickName() + " " + nickname, channelName);
            else
            {
                this->channels.find(channelName)->second.addInvited(nickname);
                sendErrRep(341, clientFd, "INVITE", this->users.find(clientFd)->second.getNickName() + " " + nickname, channelName);
            }
        }
    }
}