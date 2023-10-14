#include "Server.hpp"

/**************************************************************/
/*                         Functions                          */
/**************************************************************/


void port_check(const char *port)
{
    if (!port || *port == '\0')
        throw(std::runtime_error("Error: port range not valid"));
    char *endp;
    long conv = std::strtol(port, &endp, 10);
    if (*endp != '\0' || conv <= 1024 || conv > 65535)
        throw(std::runtime_error("Error: port range not valid"));
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
    cmds["pass"] = &Server::cmdPass; cmds["nick"] = &Server::cmdNick; cmds["notice"] = &Server::cmdNotice;
    cmds["user"] = &Server::cmdUser; cmds["topic"] = &Server::cmdTopic; cmds["invite"] = &Server::cmdInvite;
    cmds["kick"] = &Server::cmdKick; cmds["privmsg"] = &Server::cmdPrivMsg; cmds["join"] = &Server::cmdJoin;
    cmds["part"] = &Server::cmdLeave; cmds["bot"] = &Server::cmdBot; cmds["mode"] = &Server::cmdMode;
    cmds["*bot*"] = &Server::cmdAuthBot;
    std::map<std::string, void (Server::*)(int, std::string)>::iterator it;
    it = cmds.find(c);
    (it == cmds.end()) ? sendErrRep(421, clientFd, cmdName , this->users.find(clientFd)->second.getNickName(), "") : (this->*it->second)(clientFd, cmdParam);
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
    else if (code == 431)
        ss << ":irc.leet.com " << code << " " << command << this->errRep.find(code)->second << "\r\n";
    else if (code == 421)   ss << ":irc.leet.com 421 " << s1 << " " << command << " " << this->errRep.find(421)->second << "\r\n";
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
    else if (code == 341)   ss << ":irc.leet.com 341 " << s1 << " " << s2  << "\r\n";
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
