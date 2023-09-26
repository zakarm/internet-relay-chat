#include "Server.hpp"

/**************************************************************/
/*                        Constructors                        */
/**************************************************************/

Server::Server(int port, std::string password) : port(port), password(password)
{
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSocket == -1)
        std::runtime_error("Error: socket failed");
    memset(&this->serverAddr, 0, sizeof(this->serverAddr));
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port = htons(port);
    this->serverAddr.sin_addr.s_addr = INADDR_ANY;
    this->socketLen = sizeof(this->serverAddr);
    Utils::loadErrorsReplies(this->errRep, "IRC_SERVER", "ircserv", "");
}

Server::Server(const Server &sv)
{
    if (this != &sv)
    {
        this->serverSocket = sv.getServerSocket();
        this->socketLen = sv.getSocketLen();
        this->serverAddr = sv.getServerAddr();
    }
}

/**************************************************************/
/*                    getters and setters                     */
/**************************************************************/

int Server::getServerSocket() const{ return this->serverSocket;}
void Server::setServerSocket(int serverSocket){ this->serverSocket = serverSocket;}
struct sockaddr_in Server::getServerAddr() const{ return this->serverAddr;}
void Server::setServerAddr(struct sockaddr_in serverAddr){ this->serverAddr = serverAddr;}
int Server::getSocketLen() const{ return this->socketLen;}
void Server::setSocketLen(int socketLen){ this->socketLen = socketLen;}
unsigned int Server::getPort() const{ return this->port;}
void Server::setPort(unsigned int port){ this->port = port;}
std::string Server::getPassword() const{ return this->password;}
void Server::setPassword(std::string password){ this->password = password;}

/**************************************************************/
/*                         Functions                          */
/**************************************************************/

void Server::customException(std::string errorMessage)
{
    close(this->serverSocket);
    throw std::runtime_error(errorMessage);
}

void Server::runServer()
{
    socketOptions();
    noBlockingFd();
    bindServer();
    listenServer();
    this->pfds.push_back((struct pollfd){.fd = this->serverSocket, .events = POLLIN});
    this->channels.insert(std::make_pair("general", Channel("general")));
    this->channels.insert(std::make_pair("data", Channel("data")));
    multipleClients();
}

bool Server::checkDuplicateNick(std::string nickName)
{
    std::map<int, User>::iterator it;
    for (it = this->users.begin(); it != this->users.end(); it++)
    {
        if (it->second.getNickName() == nickName)
            return false;
    }
    return true;
}

bool Server::checkPass(std::string password)
{
    if (password != this->password) 
        return false;
    return true;
}

int Server::getClientFdbyNick(std::string nick)
{
    for (size_t i = 0; i < this->users.size(); i++)
    {
        if (this->users[i].getNickName() == nick)
            return this->users[i].getClientFd();
    }
    return -1;
}
/**************************************************************/
/*                         Destructor                         */
/**************************************************************/
Server::~Server()
{

}