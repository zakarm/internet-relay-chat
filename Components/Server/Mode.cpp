#include "Server.hpp"


bool is_valid_mode(char c)
{
    if (c == 'i' || c == 'o' || c == 'l' || c == 't' || c == 'k' || c == 'v')
        return true;
    return false;
}

void    Server::emptyMode(std::string channel, int clientFd)
{
    if (this->channels[channel].getMode() != 0)
    {
        std::string msg1 = ":irc.leet.com 324 " + this->users[clientFd].getNickName() + " " + channel + " " + this->channels[channel].getModeString() + "\r\n";
        send(clientFd, msg1.c_str(), msg1.size(), 0);
    }
}

void    Server::cmdMode(int clientFd, std::string cmd)
{
    char sign = '+';
    if (cmd.empty())
        {sendErrRep(650, clientFd, "MODE", "", "");return;}
    std::string channel, mode, params;   
    std::stringstream ss(cmd);
    ss >> channel;
    if (this->channels.find(channel) == this->channels.end())
        {sendErrRep(403, clientFd, "MODE", "", channel);return;}
    
    ss >> mode;
    if (mode.empty())
        {emptyMode(channel, clientFd);return;}
    std::getline(ss, params);
    std::stringstream ss2(params);
    for(size_t i = 0; i < mode.length(); i++)
    {
        if (mode[i] == '+' || mode[i] == '-')
            sign = mode[i];
        else if (is_valid_mode(mode[i]))
        {
            if (mode[i] == 'i')
                i_mode(channel, sign, clientFd);
            if (mode[i] == 'o')
                o_mode(clientFd, sign, channel, ss2);
            if (mode[i] == 'l')
                l_mode(clientFd, sign, channel, ss2);
            if (mode[i] == 't')
                t_mode(channel, sign, clientFd);
            if (mode[i] == 'k')
                k_mode(clientFd, sign, channel, ss2);
            if (mode[i] == 'v')
            {
                std::string msg( "Voice mode is not supported by this server");
                sendErrRep(696, clientFd, "MODE", channel, msg);
            }
        }
        else
            sendErrRep(472, clientFd, "MODE", std::string (1,mode[i]), channel);
    }
}


void Server::k_mode(int clientFd, char sign, std::string channel, std::stringstream &ss)
{
    Channel& chan = this->channels[channel];
    std::string key;
    if (sign == '+' && !(chan.getMode() & Channel::KEY))
    {
        std::ws (ss);
        std::getline(ss, key);
        if (!key.empty())
        {
            chan.setKey(key);
            chan.setMode(Channel::KEY);
            printModemessage(channel, "+k", clientFd);
        }
        else
        {
            std::string message = ":irc.leet.com 696 " + this->users[clientFd].getNickName() + " " + channel + " k * You must specify a parameter for the key mode. Syntax: <key>.\r\n";
            send(clientFd, message.c_str(), message.size(), 0);
        }
    }
    else if (sign == '-' && chan.getMode() & Channel::KEY)
    {
        chan.unsetMode(Channel::KEY);
        chan.setKey("");
        printModemessage(channel, "-k", clientFd);
    }
}

void Server::l_mode(int clientFd, char sign, std::string channel, std::stringstream &ss)
{
    Channel& chan = this->channels[channel];
    if (sign == '+' && !(chan.getMode() & Channel::LIMIT))
    {
        std::string limit;
        ss >> limit;
        if (limit.empty())
        {
            std::string message = ":irc.leet.com 696 " + this->users[clientFd].getNickName() + " " + channel + " l * You must specify a parameter for the limit mode. Syntax: <limit>.\r\n";
            send(clientFd, message.c_str(), message.size(), 0);
            return; 
        }
        int max_limit = 30;
        int l = std::atoi(limit.c_str());
        if (l <= 0 || l > max_limit)
        {
            std::string message = ":irc.leet.com 696 " + this->users[clientFd].getNickName() + "  " + channel + " l " + limit + " :Invalid limit mode parameter. Syntax: <limit>.\r\n";
            send(clientFd, message.c_str(), message.size(), 0);
        }
        else
        {
            chan.setLimit(l);
            chan.setMode(Channel::LIMIT);
            printModemessage(channel, "+l", clientFd);
        }
    }
    else if (sign == '-' && chan.getMode() & Channel::LIMIT)
    {
        chan.unsetMode(Channel::LIMIT);
        printModemessage(channel, "-l", clientFd);
    }
}


void    Server::i_mode(std::string channel, char sign, int clientFd)
{
    Channel& chan = this->channels[channel];
    if (sign == '+' && !(chan.getMode() & Channel::INVITE_ONLY))
    {  
        chan.setMode(chan.INVITE_ONLY);
        printModemessage(channel, "+i", clientFd);
    }
    else if (sign == '-' && chan.getMode() & Channel::INVITE_ONLY)
    {
        chan.unsetMode(chan.INVITE_ONLY);
        printModemessage(channel, "-i", clientFd);
    }
}

void Server::t_mode(std::string channel, char sign, int clientFd)
{
    Channel& chan = this->channels[channel];
    if (sign == '+' && !(chan.getMode() & Channel::TOPIC_PROTECTED))
    {
        chan.setMode(Channel::TOPIC_PROTECTED);
        printModemessage(channel, "+t", clientFd);
    }
    else if (sign == '-' && chan.getMode() & Channel::TOPIC_PROTECTED)
    {
        chan.unsetMode(Channel::TOPIC_PROTECTED);
        printModemessage(channel, "-t", clientFd);
    }
}


void Server::o_mode(int clientFd , char sign, std::string channel, std::stringstream& ss)
{
    Channel& chan = this->channels[channel];
    std::string nick;
    ss >> nick;
    if (nick.empty())
    {
        std::string message = ":irc.leet.com 696 " + this->users[clientFd].getNickName() + " " + channel + " o * You must specify a parameter for the op mode. Syntax: <nick>.\r\n";
        send(clientFd, message.c_str(), message.size(), 0);
        return;
    }
    int targetFd = getUserFdByNick(nick);
    if (targetFd == -1)
        {sendErrRep(401, clientFd, "MODE", this->users[clientFd].getNickName(), nick);return;}
    User& targetUser = this->users[targetFd];
    if (!targetUser.isInChannel(channel))
        {sendErrRep(441, clientFd, "MODE", targetUser.getNickName(), channel);return;}
    std::string message = set_operator(channel, &(this->users[targetFd]), sign == '+' ? "+o" : "-o");
    if (!message.empty())
    {
        chan.broadcast(&(this->users.find(clientFd)->second), message, &(this->responses));
        message = ":" + this->users[clientFd].getNickName() + "!~" + this->users[clientFd].getUserName() + "@" + this->users[clientFd].getHostName() + " " + message + "\r\n"; /// might need to change the sender
        send(clientFd, message.c_str(), message.size(), 0);
    }
}

std::string Server::set_operator(std::string channel, User *user, std::string mode)
{
    std::string message;
    std::map<std::string, Channel>::iterator channelIt = channels.find(channel);
    if (mode == "+o" && !channelIt->second.isOperator(user->getClientFd()))
    {       
        channelIt->second.changeOpMode(user, true);
        message = "MODE " + channel + " +o " + user->getNickName();
    }
    else if (mode == "-o" && channelIt->second.isOperator(user->getClientFd()) && channelIt->second.getOperators().size() > 1)
    {   
        channelIt->second.changeOpMode(user, false);
        message = "MODE " + channel + " -o " + user->getNickName();
    }
    return message;
}


void   Server::printModemessage(std::string channel, std::string mode, int clientFd)
{ 
    Channel& chan = this->channels[channel];
    std::string message = "MODE " + channel + " " + mode;
    chan.broadcast(&(this->users.find(clientFd)->second), message, &(this->responses));
    message = ":" + this->users[clientFd].getNickName() + "!~" + this->users[clientFd].getUserName() + "@" + this->users[clientFd].getHostName() + " " + message + "\r\n"; 
    send(clientFd, message.c_str(), message.size(), 0);
}