#include "Server.hpp"

void Server::cmdJoin(int clientFd, std::string data)
{
    if (!this->users.find(clientFd)->second.getIsConnected())
    {
        sendErrRep(451, clientFd, "JOIN", "", "");
        return;
    }
    if (data.empty())
    {
        sendErrRep(461, clientFd, "JOIN", "", "");
        return;
    }

    std::stringstream ss(data);
    std::string channels, passwords, channel, password;
    ss >> channels;
    ss >> std::ws;
    std::getline(ss, passwords);
    if (passwords[0] == ':')
        passwords = passwords.substr(1, passwords.length() - 1);
    std::stringstream ss2(channels);
    std::stringstream ss3(passwords);
    while (std::getline(ss2, channel, ','))
    {
        std::getline(ss3, password, ',');
        if (channel.empty())
        {
            sendErrRep(461, clientFd, "JOIN", "", "");
            continue;
        }
        if (channel[0] != '#' || channel.size() == 1)
        {
            std::stringstream err;
            err << ":irc.leet.com 476 JOIN " << channel << " " << this->errRep.find(476)->second << "\r\n";
            send(clientFd, err.str().c_str(), err.str().size(), 0);
            err.clear();
            continue;
        }
        if (this->channels.find(channel) == this->channels.end())
            this->channels.insert(std::make_pair(channel, Channel(channel)));
        else
        {   
            if (this->users.find(clientFd)->second.isInChannel(channel))
            {
                sendErrRep(443, clientFd, "JOIN", this->users.find(clientFd)->second.getNickName(), channel);
                continue;
            }
            if (this->channels[channel].getMode() & Channel::INVITE_ONLY && !this->channels[channel].isInvited(this->users.find(clientFd)->second.getNickName()))
            {
                sendErrRep(473, clientFd, "", this->users.find(clientFd)->second.getNickName(), channel);
                continue;
            }
            if (this->channels[channel].getMode() & Channel::KEY && this->channels[channel].getKey() != password)
            {
                sendErrRep(475, clientFd, "", this->users.find(clientFd)->second.getNickName(), channel);
                continue;
            }
            if (this->channels[channel].getMode() & Channel::LIMIT && this->channels[channel].getMemberCount() >= this->channels[channel].getLimit())
            {
                sendErrRep(471, clientFd, "", this->users.find(clientFd)->second.getNickName(), channel);
                continue;
            }
        }
        this->channels[channel].addUser(&(this->users.find(clientFd)->second));
        this->channels[channel].sendNames(clientFd, this->users.find(clientFd)->second.getNickName());
        this->channels[channel].broadcast(&(this->users.find(clientFd)->second), "JOIN " + channel, NULL, true);
    }
}