#include "Server.hpp"

/**************************************************************/
/*                         Functions                          */
/**************************************************************/

void Server::authenticate(int clientFd)
{
    User u = this->users.find(clientFd)->second;
    if (!u.getNickName().empty() && u.getSetPass())
    {
        for (int i = 1; i <= 5; i++)
            sendErrRep(i, clientFd, "", "", "");
        this->users.find(clientFd)->second.setIsConected(true);
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

void   port_check(const char *port)
{
    if (!port || *port == '\0')
        throw(std::runtime_error("Error: port range not valid"));
    char *endp;
    long conv = std::strtol(port, &endp, 10);
    if (*endp != '\0' || conv <= 0 || conv > 65535)
        throw(std::runtime_error("Error: port range not valid"));
    //check correct port range
}

void Server::cmdNick(int clientFd, std::string data)
{
    if (!this->users[clientFd].getSetPass())
        {sendErrRep(451, clientFd, "", "", ""); return ;}
    std::stringstream ss(data);
    std::string nickName;
    ss >> nickName;
    if(nickName.empty() || nickName == ":")
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
            std::string msg = ":" + oldNick + " NICK " + nickName + " ; " + oldNick + " changed his nickname to " + nickName + "\r\n";
            send(clientFd, msg.c_str(), msg.size(), 0);
        }
        this->users.find(clientFd)->second.setNickName(nickName);
        authenticate(clientFd);
    }
}

bool Server::validNick(const std::string& data)
{
    std::string::const_iterator it;
        if(!isalpha(data[0]))
        return false;
        for (it = data.begin(); it != data.end(); ++it)
            if (!isalnum(*it) && *it != '_' && *it != '[' && *it != ']' && *it != '\\' )
                return false;
    return true;
}

void Server::cmdUser(int clientFd, std::string data)
{
    int err = userCheck(data);
    if (err == 461 || err == 462)
    {
        sendErrRep(err, clientFd, "USER", "", "");
        return;
    }
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
    //authenticate(clientFd);
}

void Server::cmdTopic(int clientFd, std::string data)
{
    if (data.empty())
        sendErrRep(461, clientFd, "TOPIC", "", "");
    else if (!this->users.find(clientFd)->second.getIsConnected())
        sendErrRep(451, clientFd, "TOPIC", "", "");
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
                sendErrRep(332, clientFd, "", this->users.find(clientFd)->second.getNickName(), channelName + " " + \
                    this->channels.find(channelName)->second.getTopic());
                sendErrRep(333, clientFd, "", this->users.find(clientFd)->second.getNickName(), channelName + " " + \
                    this->users.find(clientFd)->second.getNickName() + " " + Utils::getDate());
            }
        }
        else
        {
            if (this->channels.find(channelName)->second.getMode() == 2 && !this->channels.find(channelName)->second.isOperator(clientFd))
            {
                sendErrRep(482, clientFd, "TOPIC", this->users.find(clientFd)->second.getNickName(), channelName);
                return ;
            }
            if (newTopic[0] == ':' && newTopic.size() == 1)
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
                    sendErrRep(332, it->second->getClientFd(), "", clientNick,\
                        channelName + " " + this->channels.find(channelName)->second.getTopic());
            }
            for (it = this->channels[channelName].getOperators().begin(); it != this->channels[channelName].getOperators().end(); it++)
            {
                if (it->second->getIsConnected())
                    sendErrRep(332, it->second->getClientFd(), "", clientNick,\
                            channelName + " " + this->channels.find(channelName)->second.getTopic());
            }
        }
    }
}

void Server::cmdInvite(int clientFd, std::string data)
{
    if (data.empty())
        sendErrRep(461, clientFd, "INVITE", "", "");
    else if (!this->users.find(clientFd)->second.getIsConnected())
        sendErrRep(451, clientFd, "INVITE", "", "");
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
                    sendErrRep(443, clientFd, "INVITE", this->users.find(clientFd)->second.getNickName(), channelName);
                else
                {
                    this->users.find(target)->second.inviteChannel(&(this->channels.find(channelName)->second));
                    sendErrRep(341, clientFd, "INVITE", this->users.find(clientFd)->second.getNickName(), channelName);
                }
            }
        }   
    }
}

void Server::cmdKick(int clientFd, std::string data)
{
    if (data.empty())
        sendErrRep(461, clientFd, "KICK", "", "");
    else if (!this->users.find(clientFd)->second.getIsConnected())
        sendErrRep(451, clientFd, "KICK", "", "");
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
                int target = getUserFdByNick(nickName);
                if (target != -1)
                {
                    if (!this->users.find(target)->second.isInChannel(channelName))
                        sendErrRep(441, clientFd, "KICK", this->users.find(clientFd)->second.getNickName(), channelName);
                    else
                    {
                        if (comment[0] == ':')
                        {
                            std::stringstream rep;
                            rep << ":" << this->users.find(clientFd)->second.getNickName() << " KICK " << channelName << " " << nickName;
                            send(target, rep.str().c_str(), rep.str().size(), 0);
                        }
                        this->users.find(target)->second.leaveChannel(&(this->channels.find(channelName)->second));
                    }
                }
                else
                    sendErrRep(441, clientFd, "KICK", this->users.find(clientFd)->second.getNickName(), channelName);

            }
        }
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
        else if (Utils::stolower(cmdName) == "ping")
            return ;
        else if (Utils::stolower(cmdName) == "pong")
            return ;
        else
            sendErrRep(421, clientFd, command, "", "");
    }
}

void Server::sendErrRep(int code, int clientFd, std::string command, std::string s1, std::string s2)
{
    std::stringstream ss;
    User u = this->users.find(clientFd)->second;
    if (code == 1)          ss << ":irc.leet.com 001 " << u.getNickName() << " " << this->errRep.find(1)->second    << " " << u.getNickName() << "\r\n";
    else if (code == 2)     ss << ":irc.leet.com 002 " << u.getNickName() << " " << this->errRep.find(2)->second    << " v1" << "\r\n";
    else if (code == 3)     ss << ":irc.leet.com 003 " << u.getNickName() << " " << this->errRep.find(3)->second    << " " << Utils::getDate() << "\r\n";
    else if (code == 4)     ss << ":irc.leet.com 004 " << u.getNickName() << " " << this->errRep.find(4)->second    << "\r\n";
    else if (code == 5)     ss << ":irc.leet.com 005 " << u.getNickName() << " " << this->errRep.find(5)->second    << "\r\n";
    else if (code == 431)   ss << ":irc.leet.com 431 " << command         << " " << this->errRep.find(431)->second  << "\r\n";
    else if (code == 421)   ss << ":irc.leet.com 421 " << command         << " " << this->errRep.find(421)->second  << "\r\n";
    else if (code == 331)   ss << ":irc.leet.com 331 " << s1              << " " << s2 << " " << this->errRep.find(331)->second << "\r\n";
    else if (code == 442)   ss << ":irc.leet.com 442 " << command         << " " << s1 << " " << s2 << this->errRep.find(442)->second << "\r\n";
    else if (code == 441)   ss << ":irc.leet.com 442 " << command         << " " << s1 << " " << s2 << this->errRep.find(441)->second << "\r\n";
    else if (code == 403)   ss << ":irc.leet.com 403 " << command         << " " << s1 << " " << s2 << this->errRep.find(403)->second << "\r\n";
    else if (code == 482)   ss << ":irc.leet.com 482 " << command         << " " << s1 << " " << s2 << this->errRep.find(482)->second << "\r\n";
    else if (code == 443)   ss << ":irc.leet.com 443 " << command         << " " << s1 << " " << s2 << this->errRep.find(443)->second << "\r\n";
    else if (code == 433)   ss << ":irc.leet.com 433 " << command         << " " << u.getNickName() << " " << this->errRep.find(433)->second << "\r\n";
    else if (code == 432)   ss << ":irc.leet.com 432 " << command         << " " << u.getNickName() << " " << this->errRep.find(432)->second << "\r\n";
    else if (code == 451)   ss << ":irc.leet.com 451 " << command         <<        this->errRep.find(451)->second << "\r\n";
    else if (code == 461)   ss << ":irc.leet.com 461 " << command         <<        this->errRep.find(461)->second << "\r\n";
    else if (code == 462)   ss << ":irc.leet.com 462 " << command         <<        this->errRep.find(462)->second << "\r\n";
    else if (code == 464)   ss << ":irc.leet.com 464 " << command         <<        this->errRep.find(464)->second << "\r\n";
    else if (code == 341)   ss << ":irc.leet.com 341 " << command         << " " << s1 << " " << s2  << "\r\n";
    else if (code == 332)   ss << ":irc.leet.com 332 " << s1              << " " << s2 << "\r\n";
    else if (code == 333)   ss << ":irc.leet.com 333 " << s1              << " " << s2 << "\r\n";
    send(clientFd, ss.str().c_str(), ss.str().size(), 0);
    ss.clear();
}


void Server::cmdPrivMsg(int clientFd, std::string data)
{
    if (!this->users.find(clientFd)->second.getIsConnected())
    {
        std::stringstream err;
        err << ":irc.leet.com 451 PRIVMSG " << this->errRep.find(451)->second << "\r\n";
        send(clientFd, err.str().c_str(), err.str().size(), 0);
        err.clear();
        return;
    }
    if (data.empty())
    {
        std::stringstream err;
        err << ":irc.leet.com 411 PRIVMSG " << this->errRep.find(411)->second << "\r\n";
        send(clientFd, err.str().c_str(), err.str().size(), 0);
        err.clear();
        return;
    }

    std::stringstream ss(data);
    std::string targets, message, target;

    ss >> targets;
    ss >> std::ws;
    std::getline(ss, message);

    if (message.empty())
    {
        std::stringstream err;
        err << ":irc.leet.com 412 PRIVMSG " << this->errRep.find(412)->second << "\r\n";
        send(clientFd, err.str().c_str(), err.str().size(), 0);
        err.clear();
        return;
    }
    if (message[0] == ':')
        message = message.substr(1, message.length() - 1);

    std::stringstream ss2(targets);
    while (std::getline(ss2, target, ','))
    {
        if (target.empty())
            continue;
        std::cout << "target: " << target << std::endl;
        if (target[0] == '#' || target[1] == '#')
        {
            int to;
            to = target[0] == '%' ?  1 : 0;
            target = target.substr(1 + to, target.length() - 1);
            if (this->channels.find(target) != this->channels.end())
                this->channels[target].sendToAll(this->users[clientFd].getNickName(), message, !to);
            else
            {
                std::stringstream err;
                err << ":irc.leet.com 401 PRIVMSG " << target << " " << this->errRep.find(401)->second << "\r\n";
                send(clientFd, err.str().c_str(), err.str().size(), 0);
                err.clear();
            }
        }
        else
        this->addToResponse(this->getUserFdByNick(target), ":" + this->users[clientFd].getNickName() + " PRIVMSG " + target + " :" + message + "\r\n");
    }
    std::cout << "message: " << message << std::endl;
}


///
// void Server::cmdJoin(int clientfd, std::string data)
// {
//     return;
// }

//////////////////////////////////////////////////////////
//                   cdmUser helper's                   //
//////////////////////////////////////////////////////////

int Server::countWords(std::string data)
{
    int count = 0;
    std::stringstream ss(data);
    std::string word;
    while (ss >> word)
        count++;
    return count;
}
bool Server::checkDuplicateUser(std::string username)
{
    std::map<int, User>::iterator it;
    for (it = this->users.begin(); it != this->users.end(); it++)
        if (it->second.getUserName() == username)
            return false;
    return true;
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

int Server::userCheck(std::string data)
{
    std::stringstream s(data);
    std::string username, mode, asterisk;
    s >> username >> mode >> asterisk;
    if (!check_user(username, mode, asterisk))
        return (461);
    else if (!checkDuplicateUser(username))
        return (462);
    return (1);
}