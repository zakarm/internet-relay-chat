#include "Server.hpp"

void Server::cmdKick(int clientFd, std::string data)
{
    if (!this->users.find(clientFd)->second.getIsConnected())
        sendErrRep(451, clientFd, "KICK", "", "");
    else if (data.empty())
        sendErrRep(461, clientFd, "KICK", "", "");
    else
    {
        std::stringstream ss(data);
        std::string channelName, nickName, comment;
        ss >> channelName;
        ss >> std::ws;
        ss >> nickName;
        ss >> comment;
        if (channelName.empty() || nickName.empty())
            sendErrRep(461, clientFd, "KICK", "", "");
        else if (this->channels.find(channelName) == this->channels.end())
            sendErrRep(403, clientFd, "KICK", this->users.find(clientFd)->second.getNickName(), channelName);
        else if (!this->users.find(clientFd)->second.isInChannel(channelName))
            sendErrRep(442, clientFd, "KICK", this->users.find(clientFd)->second.getNickName(), channelName);
        else if (!this->channels.find(channelName)->second.isOperator(clientFd))
            sendErrRep(482, clientFd, "KICK", this->users.find(clientFd)->second.getNickName(), channelName);
        else
        {
            std::stringstream nicknames(nickName);
            while (std::getline(nicknames, nickName, ','))
            {
                if (!nickName.empty())
                {
                    int target = getUserFdByNick(nickName);
                    if (target != -1)
                    {
                        if (!this->users.find(target)->second.isInChannel(channelName))
                            sendErrRep(441, clientFd, "KICK", this->users.find(clientFd)->second.getNickName() + " " + nickName, channelName);
                        else
                        {
                            if (!comment.empty() && comment[0] == ':')
                                comment = comment.substr(1, comment.length());
                            std::stringstream rep;
                            rep << ":" << this->users.find(clientFd)->second.getNickName() << " KICK " << channelName << " " << nickName << " " << comment << "\r\n";
                            std::map<int, User *>::const_iterator it;
                            for (it = this->channels[channelName].getUsers().begin(); it != this->channels[channelName].getUsers().end(); it++)
                                send(it->first, rep.str().c_str(), rep.str().size(), 0);
                            for (it = this->channels[channelName].getOperators().begin(); it != this->channels[channelName].getOperators().end(); it++)
                                send(it->first, rep.str().c_str(), rep.str().size(), 0);
                            this->users.find(target)->second.leaveChannel(&(this->channels.find(channelName)->second));
                            if (this->channels.find(channelName) != this->channels.end() && this->channels[channelName].getMemberCount() == 0)
                                this->channels.erase(channelName);
                        }
                    }
                    else
                        sendErrRep(441, clientFd, "KICK", this->users.find(clientFd)->second.getNickName() + " " + nickName, channelName);
                }
                else
                    sendErrRep(441, clientFd, "KICK", this->users.find(clientFd)->second.getNickName(), channelName);
            }
        }
    }
}