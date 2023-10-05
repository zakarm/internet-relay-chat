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
        std::runtime_error("Error: socket failed");
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
        std::runtime_error("Error: connect failed");
    std::cout << "Connected to server" << std::endl;
    char buffer[1024];
    bzero(buffer, sizeof(buffer));
    send(this->clientSocket, "PASS pass\r\n", 11,0);
    send(this->clientSocket, "USER bot 0 *\r\n", 16, 0);
    send(this->clientSocket, "NICK bot\r\n", 12, 0);
    while(1);
    // while (1)
    // {
        
    //     // recv(this->clientSocket, buffer, sizeof(buffer), 0);
    //     std::cout << buffer << std::endl;
    // }
}
