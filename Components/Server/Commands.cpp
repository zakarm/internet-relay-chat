#include "Server.hpp"

/**************************************************************/
/*                         Functions                          */
/**************************************************************/

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

void port_check(const char *port)
{
    if (!port || *port == '\0')
        throw(std::runtime_error("Error: port range not valid"));
    char *endp;
    long conv = std::strtol(port, &endp, 10);
    if (*endp != '\0' || conv <= 1024 || conv > 65535)
        throw(std::runtime_error("Error: port range not valid"));
}

void Server::cmdNick(int clientFd, std::string data)
{
    if (!this->users[clientFd].getSetPass())
    {
        sendErrRep(451, clientFd, "", "", "");
        return;
    }
    std::stringstream ss(data);
    std::string nickName;
    ss >> nickName;
    if (nickName.empty() || nickName == ":")
        sendErrRep(431, clientFd, "NICK", "", "");
    else if (!validNick(nickName))
        sendErrRep(432, clientFd, "NICK", "", "");
    else if (!checkDuplicateNick(nickName))
        sendErrRep(433, clientFd, "NICK", "", "");
    else
    {
        if (this->users.find(clientFd)->second.getNickName().empty())
        {
            std::string test = "Requesting the new nick \"" + nickName + "\r\n";
            send(clientFd, test.c_str(), test.size(), 0);
        }
        else
        {
            std::string oldNick = this->users.find(clientFd)->second.getNickName();
            std::string msg = ":" + oldNick + " NICK " + nickName + " : " + oldNick + " changed his nickname to " + nickName + "\r\n";
            send(clientFd, msg.c_str(), msg.size(), 0);
        }
        this->users.find(clientFd)->second.setNickName(nickName);
        authenticate(clientFd);
        this->nicks.insert(std::make_pair(nickName, clientFd));
    }
}

bool Server::validNick(const std::string &data)
{
    std::string::const_iterator it;
    if (!isalpha(data[0]))
        return false;
    for (it = data.begin(); it != data.end(); ++it)
        if (!isalnum(*it) && *it != '_' && *it != '[' && *it != ']' && *it != '\\')
            return false;
    return true;
}

bool Server::checkDuplicateUser(int clientFd)
{
  if (users.find(clientFd)->second.getUserName().empty())
        return true;
    return false;
}

bool    Server::check_user(const std::string& username, const std::string& mode, const std::string& asterisk)
{
     if (username.empty() || mode.empty() || asterisk.empty() || username == "\"\"" || username == "''")
        return false; 
    if(mode[0] != '0' || mode.length() != 1)
        return false;
    if (asterisk[0] != '*' || asterisk.length() != 1)
        return false;
    if (username.length() == 2 && username[0] == '"' && username[1] == '"')
        return false;
    // if (realname[0] != ':')
    //     return false;
    return true;
}

int Server::userCheck(std::string data, int ft_clientFd)
{
    std::stringstream s(data);
    std::string username, mode, asterisk;
    s >> username >> mode >> asterisk;
    if (!check_user(username, mode, asterisk))
            return (461);
    else if (!checkDuplicateUser(ft_clientFd))
        return (462);
    return (1);
}


void Server::cmdUser(int clientFd, std::string data)
{
    if (!this->users[clientFd].getSetPass())
    {
        sendErrRep(451, clientFd, "", "", "");
        return;
    }
    int err = userCheck(data, clientFd);
    if (err == 461 || err == 462)
    {
        sendErrRep(err, clientFd, "USER", "", "");
        return ;
    }
    else
    {   
        std::stringstream s(data);
        std::string username, mode, asterisk, realname;
        s >> username >> mode >> asterisk;
        std::getline(s, realname);
        std::string::iterator it = realname.begin();
        while(it!= realname.end() && isspace(*it))
            it++;
        realname.erase(realname.begin(), it);
        if (realname[0] == ':')
            realname = realname.substr(1, realname.length() - 1);
        this->users[clientFd].setRealName(realname);
        this->users[clientFd].setUserName(username);
    }
    authenticate(clientFd);
}

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
                sendErrRep(331, clientFd, "", this->users.find(clientFd)->second.getNickName(), channelName);
            else
            {
                sendErrRep(332, clientFd, "", this->users.find(clientFd)->second.getNickName(), channelName + " " + this->channels.find(channelName)->second.getTopic());
                sendErrRep(333, clientFd, "", this->users.find(clientFd)->second.getNickName(), channelName + " " + this->users.find(clientFd)->second.getNickName() + " " + Utils::getDate());
            }
        }
        else
        {
            if (this->channels.find(channelName)->second.getMode() == 2 && !this->channels.find(channelName)->second.isOperator(clientFd))
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
                               channelName + " " + this->channels.find(channelName)->second.getTopic());
            }
            for (it = this->channels[channelName].getOperators().begin(); it != this->channels[channelName].getOperators().end(); it++)
            {
                if (it->second->getIsConnected())
                    sendErrRep(332, it->second->getClientFd(), "", clientNick,
                               channelName + " " + this->channels.find(channelName)->second.getTopic());
            }
        }
    }
}

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
        else if (this->channels.find(channelName)->second.getMode() == 1 && !this->channels.find(channelName)->second.isOperator(clientFd))
            sendErrRep(482, clientFd, "INVITE", this->users.find(clientFd)->second.getNickName(), channelName);
        else
        {
            int target = getUserFdByNick(nickname);
            if (target != -1)
            {
                if (this->users.find(target)->second.isInChannel(channelName))
                    sendErrRep(443, clientFd, "INVITE", this->users.find(clientFd)->second.getNickName() + " " + nickname, channelName);
                else
                {
                    this->channels.find(channelName)->second.addInvited(nickname);
                    sendErrRep(341, clientFd, "INVITE", this->users.find(clientFd)->second.getNickName() + " " + nickname, channelName);
                }
            }
        }
    }
}

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
                                comment = comment.substr(0, comment.length());
                            std::stringstream rep;
                            rep << ":" << this->users.find(clientFd)->second.getNickName() << " KICK " << channelName << " " << nickName << " " << comment << "\r\n";
                            send(target, rep.str().c_str(), rep.str().size(), 0);
                            this->users.find(target)->second.leaveChannel(&(this->channels.find(channelName)->second));
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
        // if (message[0] == ':')
        //     message = message.substr(1, message.length() - 1);
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
                std::cout << "target: " << target << std::endl;
                message = target + " " + message;
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
                    this->addToResponse(this->getUserFdByNick(target), ":" + this->users[clientFd].getNickName()+ "!" + this->users[clientFd].getUserName() + "@" + this->users[clientFd].getHostName() + " PRIVMSG " + target + " :" + message + "\r\n");
            }
        }
    }
}

void Server::cmdBot(int clientFd, std::string command)
{
    if (!this->users.find(clientFd)->second.getIsConnected())
        sendErrRep(451, clientFd, "BOT", "", "");
    else if (command.empty())
    {
        std::stringstream ss;
        ss << ":BOT NOTICE BOT : state, time\r\n";
        send(clientFd, ss.str().c_str(), ss.str().size(), 0);
    }
    else if (command == "time")
    {
        std::stringstream ss;
        ss << ":BOT NOTICE time :" << Utils::getDate() << std::endl;
        send(clientFd, ss.str().c_str(), ss.str().size(), 0);
    }
    else if (command == "state")
    {
        std::stringstream ss;
        ss << ":BOT NOTICE state : users on the server = " << this->users.size() << ", channels on the server = " << this->channels.size() << " \r\n";
        send(clientFd, ss.str().c_str(), ss.str().size(), 0);
    }
}

void Server::runCommand(int clientFd, std::string command)
{
    if (!command.empty())
    {
        std::stringstream ss(command);
        std::string cmdName, cmdParam;
        ss >> cmdName;
        ss >> std::ws;
        std::getline(ss, cmdParam);
        if (cmdName.empty())
            return;
        if (Utils::stolower(cmdName) == "pass")
            cmdPass(clientFd, cmdParam);
        else if (Utils::stolower(cmdName) == "nick")
            cmdNick(clientFd, cmdParam);
        else if (Utils::stolower(cmdName) == "user")
            cmdUser(clientFd, cmdParam);
        else if (Utils::stolower(cmdName) == "quit")
            clientDisconnected(clientFd);
        else if (Utils::stolower(cmdName) == "topic")
            cmdTopic(clientFd, cmdParam);
        else if (Utils::stolower(cmdName) == "invite")
            cmdInvite(clientFd, cmdParam);
        else if (Utils::stolower(cmdName) == "kick")
            cmdKick(clientFd, cmdParam);
        else if (Utils::stolower(cmdName) == "privmsg")
            cmdPrivMsg(clientFd, cmdParam);
        else if (Utils::stolower(cmdName) == "join")
            cmdJoin(clientFd, cmdParam);
        else if (Utils::stolower(cmdName) == "part")
            cmdLeave(clientFd, cmdParam);
        else if (Utils::stolower(cmdName) == "bot")
            cmdBot(clientFd, cmdParam);
        else if (Utils::stolower(cmdName) == "mode")
            cmdMode(clientFd, cmdParam);
        else if (Utils::stolower(cmdName) == "ping")
            return;
        else if (Utils::stolower(cmdName) == "pong")
            return;
        else
            sendErrRep(421, clientFd, command, "", "");
    }
}

void Server::sendErrRep(int code, int clientFd, std::string command, std::string s1, std::string s2)
{
    std::stringstream ss;
    User u = this->users.find(clientFd)->second;
    if (code == 1)          ss << ":irc.leet.com 001 " << u.getNickName() << " " << this->errRep.find(1)->second << " " << u.getNickName() << "\r\n";
    else if (code == 2)     ss << ":irc.leet.com 002 " << u.getNickName() << " " << this->errRep.find(2)->second << " v1" << "\r\n";
    else if (code == 3)     ss << ":irc.leet.com 003 " << u.getNickName() << " " << this->errRep.find(3)->second << " " << Utils::getDate() << "\r\n";
    else if (code == 4)     ss << ":irc.leet.com 004 " << u.getNickName() << " " << this->errRep.find(4)->second << "\r\n";
    else if (code == 5)     ss << ":irc.leet.com 005 " << u.getNickName() << " " << this->errRep.find(5)->second << "\r\n";
    else if (code == 431)   ss << ":irc.leet.com 431 " << command         << this->errRep.find(431)->second << "\r\n";
    else if (code == 421)   ss << ":irc.leet.com 421 " << command         << this->errRep.find(421)->second << "\r\n";
    else if (code == 331)   ss << ":irc.leet.com 331 " << s1              << " " << s2 << this->errRep.find(331)->second << "\r\n";
    else if (code == 442)   ss << ":irc.leet.com 442 " << command         << " " << s1 << " " << s2 << this->errRep.find(442)->second << "\r\n";
    else if (code == 441)   ss << ":irc.leet.com 441 " << command         << " " << s1 << " " << s2 << this->errRep.find(441)->second << "\r\n";
    else if (code == 403)   ss << ":irc.leet.com 403 " << command         << " " << s1 << " " << s2 << this->errRep.find(403)->second << "\r\n";
    else if (code == 482)   ss << ":irc.leet.com 482 " << command         << " " << s1 << " " << s2 << this->errRep.find(482)->second << "\r\n";
    else if (code == 443)   ss << ":irc.leet.com 443 " << command         << " " << s1 << " " << s2 << this->errRep.find(443)->second << "\r\n";
    else if (code == 433)   ss << ":irc.leet.com 433 " << command         << " " << u.getNickName() << this->errRep.find(433)->second << "\r\n";
    else if (code == 432)   ss << ":irc.leet.com 432 " << command         << " " << u.getNickName() << this->errRep.find(432)->second << "\r\n";
    else if (code == 451)   ss << ":irc.leet.com 451 " << command         << this->errRep.find(451)->second << "\r\n";
    else if (code == 461)   ss << ":irc.leet.com 461 " << command         << this->errRep.find(461)->second << "\r\n";
    else if (code == 462)   ss << ":irc.leet.com 462 " << command         << this->errRep.find(462)->second << "\r\n";
    else if (code == 464)   ss << ":irc.leet.com 464 " << command         << this->errRep.find(464)->second << "\r\n";
    else if (code == 341)   ss << ":irc.leet.com 341 " << command         << " " << s1 << " " << s2  << "\r\n";
    else if (code == 332)   ss << ":irc.leet.com 332 " << s1              << " " << s2 << "\r\n";
    else if (code == 333)   ss << ":irc.leet.com 333 " << s1              << " " << s2 << "\r\n";
    else if (code == 411 || code == 412)   ss << ":irc.leet.com 411 " << command << this->errRep.find(code)->second  << "\r\n";
    else if (code == 401)   ss << ":irc.leet.com 411 " << command << " " << s1 << " " << s2 << this->errRep.find(code)->second  << "\r\n";
    else if (code == 650)   ss << ":irc.leet.com 650 " << command         << this->errRep.find(650)->second << "\r\n";
    else if (code == 472)   ss << ":irc.leet.com 472 " << command         << this->errRep.find(472)->second << "\r\n";
    send(clientFd, ss.str().c_str(), ss.str().size(), 0);
    ss.clear();
}


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
            if (message.empty())
                message = this->users.find(clientFd)->second.getNickName() + " has left " + channel;
            else if (message[0] == ':')
                message = message.substr(1, message.length() - 1);
            else
                message = ":" + this->users.find(clientFd)->second.getNickName() + " " + message;
            this->channels[channel].broadcast(&(this->users.find(clientFd)->second), "PART " + channel + " " + message, &(this->responses), true);
        }
        std::cout << this->users.find(clientFd)->second.getNickName() << " left " << channel << std::endl;
        std::cout << "channel size: " << this->channels[channel].getMemberCount() << std::endl;
        if (this->channels.find(channel) != this->channels.end() && this->channels[channel].getMemberCount() == 0)
            {this->channels.erase(channel); std::cout << "channel erased" << std::endl;}
    }
}

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
        if (channel[0] != '#')
        {
            std::stringstream err;
            err << ":irc.leet.com 476 JOIN " << channel << " " << this->errRep.find(476)->second << "\r\n";
            send(clientFd, err.str().c_str(), err.str().size(), 0);
            err.clear();
            continue;
        }
        if (this->channels.find(channel) == this->channels.end())
        {
            this->channels.insert(std::make_pair(channel, Channel(channel)));
            std::cout << "channel mode:" << this->channels[channel].getMode() << std::endl;
            // this->channels[channel].setMode(Channel::INVITE_ONLY);
        }
        else
        {   
            if (this->users.find(clientFd)->second.isInChannel(channel))
            {
                sendErrRep(443, clientFd, "JOIN", this->users.find(clientFd)->second.getNickName(), channel);
                continue;
            }
            if (this->channels[channel].getMode() & Channel::INVITE_ONLY && !this->channels[channel].isInvited(this->users.find(clientFd)->second.getNickName()))
            {
                // sendErrRep(473, clientFd, "JOIN", this->users.find(clientFd)->second.getNickName(), channel);
                std::cout << "invite" << std::endl;
                continue;
            }
            if (this->channels[channel].getMode() & Channel::KEY && this->channels[channel].getKey() != password)
            {
                // sendErrRep(475, clientFd, "JOIN", this->users.find(clientFd)->second.getNickName(), channel);
                std::cout << "key" << std::endl;
                continue;
            }
            if (this->channels[channel].getMode() & Channel::LIMIT && this->channels[channel].getMemberCount() >= this->channels[channel].getLimit())
            {
                // sendErrRep(471, clientFd, "JOIN", this->users.find(clientFd)->second.getNickName(), channel);
                std::cout << "limit" << std::endl;
                continue;
            }
        }
        this->channels[channel].addUser(&(this->users.find(clientFd)->second));
        this->channels[channel].sendNames(clientFd, this->users.find(clientFd)->second.getNickName());
        this->channels[channel].broadcast(&(this->users.find(clientFd)->second), "JOIN " + channel, &(this->responses), true);
    }
}
int count(std::string str)
{
    int count = 0;
    std::stringstream ss(str);
    std::string word;
    while (ss >> word)
        count++;
    return count;
}
void    Server::i_mode(std::string& channel, std::string& mode)
{
    Channel& chan = this->channels[channel];
    if (mode == "+i" )
        chan.setMode(chan.INVITE_ONLY);
    else if (mode == "-i")
        chan.unsetMode(chan.INVITE_ONLY);
}
int covert_to_int(std::string str)
{
    std::string::iterator it = str.begin();
    while (it != str.end() && isdigit(*it))
        it++;
    if (it != str.end())
        return -1;
    std::stringstream ss(str);
    int limit;
    ss >> limit;
    return limit;
}
void Server::l_mode(int clientFd, std::string cmd)
{
    std::string channel, mode, limit;
    std::stringstream ss(cmd);
    ss >> channel >> mode >> limit;
    int max_limit = 30;
    int l = 0;
    (void)clientFd;
    //to remove when error rep is done
    if (mode == "+l" || mode == "-l")
    {
        if (mode == "+l")
        { 

            l = covert_to_int(limit);
            if (l <= 0 || l > max_limit)
                //sendErrRep(696, clientFd, channel, limit , "limit");
                std::cout << "696 error to add" << std::endl;
            else
            {
                this->channels[channel].setLimit(l);
                this->channels[channel].setMode(Channel::LIMIT);
            }
        }
        else
            this->channels[channel].unsetMode(Channel::LIMIT);
    }
}
void Server::set_operator(std::string& channel, std::string& nick, std::string& mode)
{
    std::map<std::string, Channel>::iterator channelIt = channels.find(channel);
    if (mode == "+o")
        channelIt->second.o_plus(nick);
    else if (mode == "-o")
        channelIt->second.o_minus(nick);
}

void Server::o_mode(int clientFd, std::string cmd)
{
    std::string channel, mode, nick;
    std::stringstream ss(cmd);
    ss >> channel >> mode >> nick;
    User& user = this->users[clientFd];
    if (nick.empty())
    {
        sendErrRep(461, clientFd, "MODE", "", "");
        return;
    }
    int targetFd = getUserFdByNick(nick);
    if (targetFd == -1){
        sendErrRep(401, clientFd, "MODE", user.getNickName(), nick);
        return;}
    User& targetUser = this->users[targetFd];
    if (!targetUser.isInChannel(channel)){
        sendErrRep(441, clientFd, "MODE", targetUser.getNickName(), channel);
        return;}
    set_operator(channel, nick, mode);
}
void Server::t_mode(std::string& channel, std::string& mode)
{
    Channel& chan = this->channels[channel];
    if (mode == "+t")
        chan.setMode(Channel::TOPIC_PROTECTED);
    else if (mode == "-t")
        chan.unsetMode(Channel::TOPIC_PROTECTED);
}
// void    k
void    Server::cmdMode(int clientFd, std::string cmd)
{
    int c = count(cmd);
    std::string channel, mode;
    std::stringstream ss(cmd);
    ss >> channel >> mode;
    User& user = this->users[clientFd];
    Channel& chan = this->channels[channel];
    if (cmd.empty())
        sendErrRep(650, clientFd, "MODE", "", "");
    else if (mode.empty())
        std::cout << "in progress" << std::endl;
    else if (channel.empty())
        sendErrRep(461, clientFd, "MODE", "", "");
    else if (!user.getIsConnected())
        sendErrRep(451, clientFd, "MODE", "", "");
    else if (chan.getName() != channel)
        sendErrRep(403, clientFd, "MODE", user.getNickName(), channel);
    else if (!user.isInChannel(channel))
        sendErrRep(442, clientFd, "MODE", user.getNickName(), channel);
    else if (!chan.isOperator(clientFd))
        sendErrRep(482, clientFd, "MODE", user.getNickName(), channel);
    else if (std::strlen(mode.c_str()) == 2)
    {   
        if (c == 2 && (cmd.find("+i") != std::string::npos || cmd.find("-i") != std::string::npos))
            i_mode(channel, mode); //good
        else if (c == 3 && (cmd.find("+o") != std::string::npos || cmd.find("-o") != std::string::npos))
            o_mode(clientFd, cmd);
        else if (c <= 3 && (cmd.find("+l") != std::string::npos || cmd.find("-l") != std::string::npos))
            l_mode(clientFd, cmd);
        else if (c == 2 && (cmd.find("+t") != std::string::npos || cmd.find("-t") != std::string::npos))
            t_mode(channel, mode);
        else if (c == 3 && (cmd.find("+k") != std::string::npos) || cmd.find("-k") != std::string::npos)
            std::cout << "in progress !" << std::endl;
        else
            sendErrRep(472, clientFd, "MODE", user.getNickName(), channel);
    }
    else
        sendErrRep(472, clientFd, "MODE", user.getNickName(), channel);
}
///////////////////////////////////////////
// mode k are next to push           //
// error to add 696                      //
// to add mode response for empty mode   //
// to modify user input sytax            //
////////////////////////////////////////// 