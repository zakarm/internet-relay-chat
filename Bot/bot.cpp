#include "bot.hpp"

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

Bot::Bot(const Bot &bot)
{
    
    // if (this != &bot)
    // {
    //     this->clientSocket = bot.getClientSocket();
    //     this->clientAddr = bot.getClientAddr();
    //     this->socketLen = bot.getSocketLen();
    //     this->port = bot.getPort();
    //     this->password = bot.getPassword();
    //     this->nickName = bot.getNickName();
    //     this->userName = bot.getUserName();
    //     this->realName = bot.getRealName();
    //     // this->serverName = bot.getServerName();
    // }
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
    while (1)
    send (this->clientSocket, "hello from client this is a long string test: aksjdklajskldjklasjdklajsdkljaklsdjasjdkljaklsdjasfkbahlsbdfjb", 108, 0);
    
}


