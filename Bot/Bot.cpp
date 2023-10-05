#include "Bot.hpp"

Bot::Bot(int port, std::string address, std::string password)
{
    this->port = port;
    this->password = password;
    this->nickName = "Bot";
    this->userName = "Bot";
    this->realName = "Bot";
    this->serverName = "Bot";
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
        if (err != -1)
            std::cout << buffer;
        else
        {
            close(this->clientSocket);
            exit(EXIT_FAILURE);
        }
    }
}
