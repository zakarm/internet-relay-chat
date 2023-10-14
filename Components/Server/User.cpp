#include "Server.hpp"
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
            std::ws(s);
            std::getline(s, realname);
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