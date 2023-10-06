#include "Bot.hpp"

Bot::Bot(int port, std::string address, std::string password)
{
    this->port = port;
    this->password = password;
    this->nickName = "Bot";
    this->userName = "Bot";
    this->realName = "Bot";
    this->serverName = "Bot";
    this->lastSenderNick = "";
    this->socketLen = sizeof(this->clientAddr);
    this->clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->clientSocket < 0)
        std::cerr << RED << "Error: socket failed" << std::endl;
    this->clientAddr.sin_family = AF_INET;
    this->clientAddr.sin_port = htons(this->port);
    this->clientAddr.sin_addr.s_addr = inet_addr(address.c_str());
    this->connectToServer();
}

Bot::~Bot()
{
    close(this->clientSocket);
}

/**************************************************************/
/*                         Functions                          */
/**************************************************************/

void Bot::connectToServer()
{
    if (connect(this->clientSocket, (struct sockaddr *)&this->clientAddr, this->socketLen) < 0)
    {
        std::cerr << RED << "Error: connect failed" << DEFAULT << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string pass = "PASS bot\r\n";
    send(this->clientSocket, pass.c_str(), pass.size(),0);
    for (;;)
    {   
        char buffer[1024];
        bzero(buffer, sizeof(buffer));
        int err = recv(this->clientSocket, buffer, sizeof(buffer), 0);
        if (err > 0)
            runCommand(buffer);
        else
        {
            close(this->clientSocket);
            exit(EXIT_FAILURE);
        }
    }
}

void Bot::runCommand(std::string data)
{
    std::stringstream ss(data);
    std::string param, command, client, target, message;
    ss >> param >> std::ws >> command >> std::ws >> client >> std::ws >> target >> std::ws;
    std::getline(ss, message);
    std::cout << param << " " << command << std::endl;
    if (param == ":irc.leet.com" && command == "401")
    {
        std::stringstream err;
        err << "PRIVMSG " << this->lastSenderNick << " :BOT No such nick " << "\r\n";
        send(this->clientSocket, err.str().c_str(), err.str().size(), 0);
    }
    else if (command == "PRIVMSG")
    {
        std::stringstream err;
        err << "PRIVMSG ";
        if (!target.empty() && !message.empty())
        {
            if (target[0] == ':')
                target = target.substr(1, target.size());
            this->lastSenderNick = param.substr(1, param.find('!') - 1);
            err << target << " :[Anonymous Message] " << message << "\r\n";
        }
        else
            err << param.substr(1, param.find('!') - 1).c_str() << " :BOT Empty target or message " << "\r\n";
        send(this->clientSocket, err.str().c_str(), err.str().size(), 0);
    }
}