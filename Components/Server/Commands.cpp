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
    else if (!checkPass(data) && data != "bot")
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
int Server::count(std::string str)
{
    int count = 0;
    std::stringstream ss(str);
    std::string word;
    while (ss >> word)
        count++;
    return count;
}
bool Server::checkDuplicateUser(int clientFd)
{
  if (users.find(clientFd)->second.getUserName().empty())
        return true;
    return false;
}
int Server::userCheck(std::string data, int ft_clientFd)
{
    std::stringstream s(data);
    std::string username, mode, asterisk;
    s >> username >> mode >> asterisk;
    int wordCount = count(data);
     if (username.empty() || username == "\"\"" || username == "''")
            return (461);
    else if (!checkDuplicateUser(ft_clientFd))
        return (462);
    else if (wordCount != 4 && wordCount != 1)
        return 461;
    return (1);
}

void Server::cmdUser(int clientFd, std::string data)
{
    if (!this->users[clientFd].getSetPass())
    {
        sendErrRep(451, clientFd, "USER", "", "");
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
        if (count(data) == 4 )
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
        else if (count(data)  == 1)
        {
            std::stringstream s(data);
            std::string username;
            s >> username;
            this->users[clientFd].setUserName(username);
        }
        authenticate(clientFd);
    }
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
                    // this->addToResponse(this->getUserFdByNick(target), ":" + this->users[clientFd].getNickName()+ "!" + this->users[clientFd].getUserName() + "@" + this->users[clientFd].getHostName() + " PRIVMSG " + target + " :" + message + "\r\n");
            }
        }
    }
}

void Server::cmdBot(int clientFd, std::string command)
{
    if (!this->users.find(clientFd)->second.getIsConnected())
        sendErrRep(451, clientFd, "BOT", "", "");
    else 
    {
        std::ostringstream ss;
        ss << ":BOT NOTICE ";
        if (command.empty()) 
            ss << "BOT : state, time\r\n";
        else if (command == "time")
            ss << "time :" << Utils::getDate() << std::endl;
        else if (command == "state") 
            ss << "state : users on the server = " << this->users.size()
            << ", channels on the server = " << this->channels.size() << " \r\n";
        send(clientFd, ss.str().c_str(), static_cast<int>(ss.str().size()), 0);
    }
}

void Server::cmdAuthBot(int clientFd, std::string command)
{
    
    if (command == "*bot*")
    {
        if (this->users.find(clientFd) != this->users.end())
        {
            if (!checkDuplicateNick("bot"))
            {
                sendErrRep(433, clientFd, "NICK", "", "");
                return ;
            }
        }
        this->users[clientFd].setNickName("bot");
        this->users[clientFd].setUserName("bot");
        this->users[clientFd].setRealName("bot");
        this->nicks.insert(std::make_pair("bot", clientFd));
        authenticate(clientFd);
    }
}

void Server::runCommand(int clientFd, std::string command)
{
    if (command.empty())
        return ;
    std::stringstream ss(command);
    std::string cmdName, cmdParam;
    ss >> cmdName >> std::ws;
    std::getline(ss, cmdParam);
    std::string c = Utils::stolower(cmdName);
    if (c == "pong" || c == "ping")
        return ;
    else if (c == "quit")
        return clientDisconnected(clientFd), void();
    std::map<std::string, void (Server::*)(int, std::string)> cmds;
    cmds["pass"] = &Server::cmdPass; cmds["nick"] = &Server::cmdNick;
    cmds["user"] = &Server::cmdUser; cmds["topic"] = &Server::cmdTopic; cmds["invite"] = &Server::cmdInvite;
    cmds["kick"] = &Server::cmdKick; cmds["privmsg"] = &Server::cmdPrivMsg; cmds["join"] = &Server::cmdJoin;
    cmds["part"] = &Server::cmdLeave; cmds["bot"] = &Server::cmdBot; cmds["mode"] = &Server::cmdMode;
    cmds["*bot*"] = &Server::cmdAuthBot;
    std::map<std::string, void (Server::*)(int, std::string)>::iterator it;
    it = cmds.find(c);
    (it == cmds.end()) ? sendErrRep(421, clientFd, command, "", "") : (this->*it->second)(clientFd, cmdParam);
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
    else if (code == 431 || code == 421)
        ss << ":irc.leet.com " << code << " " << command << this->errRep.find(code)->second << "\r\n";
    else if (code == 421)   ss << ":irc.leet.com 421 " << command << this->errRep.find(421)->second << "\r\n";
    else if (code == 424)   ss << ":irc.leet.com 427 " << s1 << s2 << "\r\n";
    else if (code == 331)   ss << ":irc.leet.com 331 " << command << " " << s1 << " " << s2 << this->errRep.find(331)->second << "\r\n";
    else if (code == 442)   ss << ":irc.leet.com 442 " << s1 << " " << s2 << this->errRep.find(442)->second << "\r\n";
    else if (code == 441 || code == 403 || code == 482 || code == 443)   
        ss << ":irc.leet.com " << code << " " << command << " " << s1 << " " << s2 << this->errRep.find(code)->second << "\r\n";
    else if (code == 433)   ss << ":irc.leet.com 433 " << command << " " << u.getNickName() << this->errRep.find(433)->second << "\r\n";
    else if (code == 432)   ss << ":irc.leet.com 432 " << command << " " << u.getNickName() << this->errRep.find(432)->second << "\r\n";
    else if (code == 451)   ss << ":irc.leet.com 451 " << command << this->errRep.find(451)->second << "\r\n";
    else if (code == 461)   ss << ":irc.leet.com 461 " << command << this->errRep.find(461)->second << "\r\n";
    else if (code == 462)   ss << ":irc.leet.com 462 " << command << this->errRep.find(462)->second << "\r\n";
    else if (code == 464)   ss << ":irc.leet.com 464 " << command << this->errRep.find(464)->second << "\r\n";
    else if (code == 341)   ss << ":irc.leet.com 341 " << command << " " << s1 << " " << s2  << "\r\n";
    else if (code == 332)   ss << ":irc.leet.com 332 " << s1 << " " << s2 << "\r\n";
    else if (code == 333)   ss << ":irc.leet.com 333 " << s1 << " " << s2 << "\r\n";
    else if (code == 411 || code == 412)   ss << ":irc.leet.com " << code << command << this->errRep.find(code)->second  << "\r\n";
    else if (code == 401)   ss << ":irc.leet.com 401 " << s1 << " " << s2 << this->errRep.find(code)->second  << "\r\n";
    else if (code == 650)   ss << ":irc.leet.com 650 " << command <<this->errRep.find(650)->second << "\r\n";
    else if (code == 472)   ss << ":irc.leet.com 472 " << command << " " << s1 << this->errRep.find(472)->second << "\r\n";
    else if (code == 467)   ss << ":irc.leet.com 467 " << command << " " << s1 << " " << s2 << this->errRep.find(467)->second << "\r\n";
    else if (code == 473 || code == 475 || code == 471)
        ss << ":irc.leet.com " << code << " " << s1 << " " << s2  << this->errRep.find(code)->second << "\r\n";
    else if (code == 696) 
        ss << ":irc.leet.com 696 " << s1 << " " << s2 << "\r\n";
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
            std::string msg = ":" + this->users[clientFd].getNickName() + "!~" + this->users[clientFd].getUserName() + "@" + this->users[clientFd].getHostName() + " PART " + this->channels[channel].getName() + "\r\n";
            send(clientFd, msg.c_str(), msg.size(), 0);
            this->channels[channel].broadcast(&(this->users.find(clientFd)->second), "PART " + channel + " :" + message, &(this->responses), true);
            if (this->channels.find(channel) != this->channels.end() && this->channels[channel].getMemberCount() == 0)
                this->channels.erase(channel);
        }
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
void    Server::i_mode(std::string channel, std::string mode, int clientFd)
{
    Channel& chan = this->channels[channel];
    if (mode == "+i" )
    {  
        chan.setMode(chan.INVITE_ONLY);
        printModemessage(channel, "+i", clientFd);
    }
    else if (mode == "-i")
    {
        chan.unsetMode(chan.INVITE_ONLY);
        printModemessage(channel, "-i", clientFd);
    }
}

void Server::l_mode(int clientFd, std::string cmd)
{
    std::string channel, mode, limit;
    std::stringstream ss(cmd);
    ss >> channel >> mode >> limit;
    int max_limit = 30;
    int l = 0;
    if (limit.empty() && mode == "+l")
    {
        std::string message = ":irc.leet.com 696 " + this->users[clientFd].getNickName() + " " + channel + " l * You must specify a parameter for the limit mode. Syntax: <limit>.\r\n";
        send(clientFd, message.c_str(), message.size(), 0);
        return; 
    }
    if (mode == "+l" || mode == "-l")
    {
        if (mode == "+l")
        { 

            l = std::atoi(limit.c_str());
            if (l <= 0 || l > max_limit)
            {
                std::string message = ":irc.leet.com 696 " + this->users[clientFd].getNickName() + "  " + channel + " l " + limit + " :Invalid limit mode parameter. Syntax: <limit>.\r\n";
                send(clientFd, message.c_str(), message.size(), 0);
            }
            else
            {
                this->channels[channel].setLimit(l);
                this->channels[channel].setMode(Channel::LIMIT);
                printModemessage(channel, "+l", clientFd);
            }
        }
        else if (mode == "-l" && this->channels[channel].getMode() & Channel::LIMIT)
        {
            this->channels[channel].unsetMode(Channel::LIMIT);
            printModemessage(channel, "-l", clientFd);
        }
    }
}
std::string Server::set_operator(std::string channel, User *user, std::string mode)
{
    std::string message;
    std::map<std::string, Channel>::iterator channelIt = channels.find(channel);
    if (mode == "+o")
    {       
        channelIt->second.changeOpMode(user, true);
        message = "MODE " + channel + " +o " + user->getNickName();
    }
    else if (mode == "-o" && channelIt->second.getOperators().size() > 1)
    {   
        channelIt->second.changeOpMode(user, false);
        message = "MODE " + channel + " -o " + user->getNickName();
    }
    return message;
}

void Server::o_mode(int clientFd, std::string cmd)
{

    std::string channel, mode, nick , message;
    std::stringstream ss(cmd);
    ss >> channel >> mode >> nick;
    User& user = this->users[clientFd];
    if (nick.empty())
    {
        std::string message = ":irc.leet.com 696 " +this->users[clientFd].getNickName() + " "+ channel + " o * You must specify a parameter for the op mode. Syntax: <nick>.\r\n";
        send(clientFd, message.c_str(), message.size(), 0);
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
    message = set_operator(channel, &(this->users[targetFd]), mode);
    if (!message.empty())
    {
        this->channels[channel].broadcast(&(this->users.find(clientFd)->second), message, &(this->responses));
        message = ":" + this->users[clientFd].getNickName() + "!~" + this->users[clientFd].getUserName() + "@" + this->users[clientFd].getHostName() + " " + message + "\r\n"; /// might need to change the sender
        send(clientFd, message.c_str(), message.size(), 0);
    }
}


void Server::t_mode(std::string channel, std::string mode, int clientFd)
{
    Channel& chan = this->channels[channel];
    if (mode == "+t")
    {
        chan.setMode(Channel::TOPIC_PROTECTED);
        printModemessage(channel, "+t", clientFd);
    }
    else if (mode == "-t")
    {
        chan.unsetMode(Channel::TOPIC_PROTECTED);
        printModemessage(channel, "-t", clientFd);
    }
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
    int c = count(cmd);
    std::string channel, mode;
    std::stringstream ss(cmd);
    ss >> channel >> mode;
    User& user = this->users[clientFd];
    Channel& chan = this->channels[channel];
    if (cmd.empty())
        sendErrRep(650, clientFd, "MODE", "", "");
    else if (chan.getName() != channel)
        sendErrRep(403, clientFd, "MODE", "" , channel);
    else if (mode.empty())
        emptyMode(channel, clientFd);
    else if (!user.isInChannel(channel))
        sendErrRep(442, clientFd, "MODE", user.getNickName(), channel);
    else if (!user.getIsConnected())
        sendErrRep(451, clientFd, "MODE", "", "");
    else if (!chan.isOperator(clientFd))
        sendErrRep(482, clientFd, "MODE", user.getNickName(), channel);
    else if (std::strlen(mode.c_str()) == 2)
    {   
        if (c == 2 && (cmd.find("+i") != std::string::npos || cmd.find("-i") != std::string::npos))
            i_mode(channel, mode, clientFd);
        else if (c <= 3 && (cmd.find("+o") != std::string::npos || cmd.find("-o") != std::string::npos))
            o_mode(clientFd, cmd);
        else if (c >= 2 && (cmd.find("+l") != std::string::npos || cmd.find("-l") != std::string::npos))
            l_mode(clientFd, cmd);
        else if (c == 2 && (cmd.find("+t") != std::string::npos || cmd.find("-t") != std::string::npos))
            t_mode(channel, mode, clientFd);
        else if (c >= 2 && (cmd.find("+k") != std::string::npos || cmd.find("-k") != std::string::npos))
            k_mode(cmd, clientFd);
        else
            sendErrRep(472, clientFd, "MODE", mode , channel);
    }
    else
        sendErrRep(472, clientFd, "MODE", mode, channel);
}

void    Server::k_mode(std::string cmd, int clientFd)
{
    std::string channel, mode, key;
    std::stringstream ss(cmd);
    ss >> channel >> mode;
    std::getline(ss, key);
    Channel& chan = this->channels[channel];
    if (!key.empty())
    {
        if (mode == "+k")
        {
            chan.setKey(key);
            chan.setMode(Channel::KEY);
            printModemessage(channel, "+k", clientFd);
        }
        else if (mode == "-k" && chan.getMode() & Channel::KEY)
        {
            if (std::strcmp(chan.getKey().c_str(), key.c_str()) == 0)
            {
                chan.unsetMode(Channel::KEY);
                chan.setKey(""); 
                printModemessage(channel, "-k", clientFd);
            }
            else
                sendErrRep(467, clientFd, "MODE", "", channel);
        }
    }
    else
    {
        std::string message = ":irc.leet.com 696 " + this->users[clientFd].getNickName() + " " + channel + " k * You must specify a parameter for the key mode. Syntax: <key>.\r\n";
        send(clientFd, message.c_str(), message.size(), 0);
    }
}

void   Server::printModemessage(std::string channel, std::string mode, int clientFd)
{ 
    Channel& chan = this->channels[channel];
    std::string message = "MODE " + channel + " " + mode;
    chan.broadcast(&(this->users.find(clientFd)->second), message, &(this->responses));
    message = ":" + this->users[clientFd].getNickName() + "!~" + this->users[clientFd].getUserName() + "@" + this->users[clientFd].getHostName() + " " + message + "\r\n"; 
    send(clientFd, message.c_str(), message.size(), 0);
}