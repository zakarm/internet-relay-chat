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

void Server::noBlockingFd()
{
    if (fcntl(this->serverSocket, F_SETFL, O_NONBLOCK) == -1)
        customException("Error : fcntl failed");
}

void Server::socketOptions()
{
    int reuse = 1;
    if (setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse,
                   sizeof(reuse)) == -1)
        customException("Error: setsockopt SO_REUSEADDR failed");
}

void Server::bindServer()
{
    if (bind(this->serverSocket, (const sockaddr *)&(this->serverAddr),
             this->socketLen) == -1)
        customException("Error : bind failed");
}

void Server::listenServer()
{
    if (listen(this->serverSocket, SOMAXCONN) == -1)
        customException("Error : listen failed");
}

void Server::clientDisconnected(int clientFd)
{

    close(clientFd);
    std::cout << Utils::getTime() << " " << this->users.at(clientFd).getNickName() << " has left the server." << std::endl;
    if (this->buffring.find(clientFd) != this->buffring.end()) 
        this->buffring.erase(this->buffring.find(clientFd));
    if (this->users.find(clientFd) != this->users.end())
        this->users.erase(this->users.find(clientFd));
    
    for (std::vector<struct pollfd>::iterator it = this->pfds.begin(); it != this->pfds.end(); it++)
    {
        if (it->fd == clientFd)
        {
            this->pfds.erase(it);
            break;
        }
    }
}

std::string Server::joinBuffers(int indexClient, char *buffer)
{
    
    if (buffer[strlen(buffer) - 1] != '\n')
        this->buffring[this->pfds[indexClient].fd] += buffer;
    else
    {
        this->buffring[this->pfds[indexClient].fd] += buffer;
        std::string command = this->buffring[this->pfds[indexClient].fd];
        this->buffring[this->pfds[indexClient].fd].clear();
        if (command.find('\n') != std::string::npos)
            command = command.substr(0, command.find('\n'));
        if (command.find('\r') != std::string::npos)
            command = command.substr(0, command.find('\r'));
        return (command);
    }
    return ("");
}

void Server::requests(int indexClient)
{   
    if (this->pfds[indexClient].revents & (POLLHUP | POLL_ERR))
        clientDisconnected(this->pfds[indexClient].fd);
    else if (this->pfds[indexClient].revents & POLLIN)
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int r = recv(this->pfds[indexClient].fd, buffer, sizeof(buffer), 0);
        std::string str;
        if (r <= 0)
            clientDisconnected(this->pfds[indexClient].fd);
        else
            runCommand(this->pfds[indexClient].fd, joinBuffers(indexClient, buffer));
    }
    else if (this->pfds[indexClient].revents & POLLOUT)
    {
        if (!this->responses.empty())
        {
            std::pair<int, std::string> response = this->responses.front();
            this->responses.pop();
            send(response.first, response.second.c_str(), response.second.length(), 0);
        }
    }

}

void Server::acceptAndDecline()
{
    static int number = 0;
    struct sockaddr_in clientAddr;
    unsigned int csocketLen = sizeof(clientAddr);
    int client_fd = accept(this->serverSocket, (sockaddr *)&clientAddr, (socklen_t *)&csocketLen);
    if (client_fd == -1)
    {
        std::cerr << RED << "Error : accept failed (max_fd reached)" << DEFAULT << std::endl;
        return ;
    }
    std::cout << Utils::getTime() << " " << inet_ntoa(clientAddr.sin_addr)
                << " has joined the server." << std::endl;
    this->pfds.push_back((struct pollfd){.fd = client_fd, .events = POLLIN});
    this->users.insert(std::make_pair(client_fd, User(client_fd)));
    this->users[client_fd].setNickName("user" + std::to_string(number++));
    this->users[client_fd].joinChannel(&this->channels["general"]);
    // this->channels["general"].listUsers();
    // std::cout << Utils::getTime()<< " " << this->channels["general"].getUserNickByFd(client_fd) << " has joined the channel " << this->channels["general"].getName() << std::endl;

}

void Server::multipleClients()
{
    for (;;)
    {
        int numfds = poll(&(this->pfds[0]), this->pfds.size(), POLL_TIMEOUT );
        if (numfds == -1)
            customException("Error : poll failed");
        if (numfds == 0)
            continue;
        if (this->pfds[0].revents & POLLIN)
            acceptAndDecline();
        else
            for (size_t i = 0; i < this->pfds.size(); i++)
                requests(i);
    }
}

void Server::runServer()
{
    socketOptions();
    noBlockingFd();
    bindServer();
    listenServer();
    this->pfds.push_back((struct pollfd){.fd = this->serverSocket, .events = POLLIN});
    this->channels.insert(std::make_pair("general", Channel("general")));
    multipleClients();
}

bool Server::checkPass(std::string password)
{
    if (password != this->password) 
        return false;
    return true;
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

void Server::cmdPass(int clientFd, std::string data)
{
    std::stringstream err;
    if (data.empty())
        sendErrRep(461, clientFd, "PASS", "", "");
    else if (this->users.find(clientFd)->second.getIsConnected())
        sendErrRep(462, clientFd, "PASS", "", "");
    else if (!checkPass(data))
        sendErrRep(464, clientFd, "PASS", "", "");
    else {
        this->users.find(clientFd)->second.setSetPass(true);
        authenticate(clientFd);
    }
}

void Server::cmdNick(int clientFd, std::string data)
{
    // std::cout << "fd:" << clientFd << " " << data << std::endl;
    (void)clientFd;
    (void)data;
}

void Server::cmdUser(int clientFd, std::string data)
{
    // std::cout << "fd:" << clientFd << " " << data << std::endl;
    (void)clientFd;
    (void)data;
}

void Server::cmdTopic(int clientFd, std::string data)
{
    std::stringstream err;
    std::stringstream rep;
    if (data.empty())
        sendErrRep(461, clientFd, "TOPIC", "", "");
    else if (!this->users.find(clientFd)->second.getIsConnected())
        sendErrRep(451, clientFd, "TOPIC", "", "");
    else
    {
        std::stringstream ss(data);
        std::string channelName, newTopic;
        ss >> channelName;
        ss >> newTopic;
        std::cout << (this->channels.find(channelName) == this->channels.end()) << std::endl;
        if (this->channels.find(channelName) == this->channels.end())
            sendErrRep(442, clientFd, "TOPIC", this->users.find(clientFd)->second.getNickName(), this->channels.find(channelName)->second.getName());
        if (this->channels.find(channelName) != this->channels.end() && newTopic.empty())
        {
            if (this->channels.find(channelName)->second.getTopic().empty())
                sendErrRep(331, clientFd, "TOPIC", this->users.find(clientFd)->second.getNickName(), this->channels.find(channelName)->second.getName());
        }
    }
        
    
}

void Server::authenticate(int clientFd)
{
    this->users.find(clientFd)->second.setNickName("Zakariae");
    this->users.find(clientFd)->second.setUserName("Apollo");
    User u = this->users.find(clientFd)->second;
    if (!u.getUserName().empty() && !u.getNickName().empty() && u.getSetPass())
    {
        for (int i = 1; i <= 5; i++)
            sendErrRep(i, clientFd, "", "", "");
    }
}

void Server::runCommand(int clientFd, std::string command)
{
    // std::cout << Utils::getTime() << " " << command << std::endl;
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
    if (code == 1) ss << ":irc.leet.com 001 " <<  u.getNickName() << " " << this->errRep.find(1)->second << " " << u.getNickName() << "\r\n";
    else if (code == 2) ss << ":irc.leet.com 002 " << u.getNickName() << " " << this->errRep.find(2)->second << " v1" << "\r\n";
    else if (code == 3) ss << ":irc.leet.com 003 " << u.getNickName() << " " << this->errRep.find(3)->second << " " << Utils::getDate() << "\r\n";
    else if (code == 4) ss << ":irc.leet.com 004 " << u.getNickName() << " " << this->errRep.find(4)->second  << "\r\n";
    else if (code == 5) ss << ":irc.leet.com 005 " << u.getNickName() << " " << this->errRep.find(5)->second << "\r\n";
    else if (code == 461) ss << ":irc.leet.com 461 " << command << this->errRep.find(461)->second << "\r\n";
    else if (code == 462) ss << ":irc.leet.com 462 " << command << this->errRep.find(462)->second << "\r\n";
    else if (code == 464) ss << ":irc.leet.com 464 " << command << this->errRep.find(464)->second << "\r\n";
    else if (code == 421) ss << ":irc.leet.com 421 " << command << " " << this->errRep.find(421)->second << "\r\n";
    else if (code == 331) ss << ":irc.leet.com 331 " << command << " " << s1 << " " << s2 << " " << this->errRep.find(331)->second << "\r\n";
    else if (code == 442) ss << ":irc.leet.com 442 " << command << " " <<s1 << " " << s2 << " " << this->errRep.find(442)->second << "\r\n";
    send(clientFd, ss.str().c_str(), ss.str().size(), 0);
    ss.clear();
}

/**************************************************************/
/*                         Destructor                         */
/**************************************************************/
Server::~Server()
{

}