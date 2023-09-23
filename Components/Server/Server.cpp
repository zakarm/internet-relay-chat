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
    loadErrorsReplies();
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

void Server::clientDisconnected(int indexClient)
{

    close(this->pfds[indexClient].fd);
    std::cout << Utils::getTime() << this->users.at(this->pfds[indexClient].fd).getNickName() << "has left the server." << std::endl;

    if (this->buffring.find(this->pfds[indexClient].fd) != this->buffring.end()) 
        this->buffring.erase(this->buffring.find(this->pfds[indexClient].fd));
    if (this->users.find(this->pfds[indexClient].fd) != this->users.end())
        this->users.erase(this->users.find(this->pfds[indexClient].fd));
    
    this->pfds.erase(this->pfds.begin() + indexClient);
    this->channels["general"].listUsers();
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
        return (command);
    }
    return ("");
}

void Server::requests(int indexClient)
{   
    if (this->pfds[indexClient].revents & (POLLHUP | POLL_ERR))
        clientDisconnected(indexClient);
    else if (this->pfds[indexClient].revents & POLLIN)
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int r = recv(this->pfds[indexClient].fd, buffer, sizeof(buffer), 0);
        std::string str;
        if (r <= 0)
            clientDisconnected(indexClient);
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

void Server::acceptUsers()
{
    for (;;)
    {
        int numfds = poll(&(this->pfds[0]), this->pfds.size(), POLL_TIMEOUT );
        if (numfds == -1)
            customException("Error : poll failed");
        if (numfds == 0)
            continue;
        if (this->pfds[0].revents & POLLIN)
        {
            static int number = 0;
            struct sockaddr_in clientAddr;
            unsigned int csocketLen = sizeof(clientAddr);
            int client_fd = accept(this->serverSocket, (sockaddr *)&clientAddr, (socklen_t *)&csocketLen);
            std::cout << Utils::getTime() << " " << inet_ntoa(clientAddr.sin_addr)
                      << " has joined the server." << std::endl;
            if (client_fd == -1)
                customException("Error : accept failed");
            this->pfds.push_back((struct pollfd){.fd = client_fd, .events = POLLIN});
            this->users.insert(std::make_pair(client_fd, User(client_fd)));
            this->users[client_fd].setNickName("user" + std::to_string(number++));
            this->users[client_fd].joinChannel(&this->channels["general"]);
            this->channels["general"].listUsers();
            // std::cout << Utils::getTime()<< " " << this->channels["general"].getUserByFd(client_fd) << " has joined the channel " << this->channels["general"].getName() << std::endl;
        }
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
    acceptUsers();
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

void Server::loadErrorsReplies()
{
    this->errRep.insert(std::make_pair(401, " No such nick/channel"));
    this->errRep.insert(std::make_pair(402, " No such server"));
    this->errRep.insert(std::make_pair(403, " No such channel"));
    this->errRep.insert(std::make_pair(404, " Cannot send to channel"));
    this->errRep.insert(std::make_pair(405, " You have joined too many channels"));
    this->errRep.insert(std::make_pair(406, " There was no such nickname"));
    this->errRep.insert(std::make_pair(409, " No origin specified"));
    this->errRep.insert(std::make_pair(411, " No recipient given"));
    this->errRep.insert(std::make_pair(412, " No text to send"));
    this->errRep.insert(std::make_pair(417, " Input line was too long"));
    this->errRep.insert(std::make_pair(421, " Unknown command"));
    this->errRep.insert(std::make_pair(422, " MOTD File is missing"));
    this->errRep.insert(std::make_pair(431, " No nickname given"));
    this->errRep.insert(std::make_pair(432, " Erroneus nickname"));
    this->errRep.insert(std::make_pair(433, " Nickname is already in use"));
    this->errRep.insert(std::make_pair(436, " Nickname collision KILL from"));
    this->errRep.insert(std::make_pair(441, " They aren't on that channel"));
    this->errRep.insert(std::make_pair(442, " You're not on that channel"));
    this->errRep.insert(std::make_pair(443, " is already on channel"));
    this->errRep.insert(std::make_pair(451, " You have not registered"));
    this->errRep.insert(std::make_pair(461, " Not enough parameters."));
    this->errRep.insert(std::make_pair(462, " You may not reregister."));
    this->errRep.insert(std::make_pair(464, " Password incorrect"));
    this->errRep.insert(std::make_pair(465, " You are banned from this server."));
    this->errRep.insert(std::make_pair(471, " Cannot join channel (+l)"));
    this->errRep.insert(std::make_pair(472, " is unknown mode char to me"));
    this->errRep.insert(std::make_pair(473, " Cannot join channel (+i)"));
    this->errRep.insert(std::make_pair(474, " Cannot join channel (+b)"));
    this->errRep.insert(std::make_pair(475, " Cannot join channel (+k)"));
    this->errRep.insert(std::make_pair(476, " Bad Channel Mask"));
    this->errRep.insert(std::make_pair(481, " Permission Denied- You're not an IRC operator"));
    this->errRep.insert(std::make_pair(482, " You're not channel operator"));
    this->errRep.insert(std::make_pair(483, " You cant kill a server!"));
    this->errRep.insert(std::make_pair(491, " No O-lines for your host"));
    this->errRep.insert(std::make_pair(501, " Unknown MODE flag"));
    this->errRep.insert(std::make_pair(502, " Cant change mode for other users"));
    this->errRep.insert(std::make_pair(524, " No help available on this topic"));
    this->errRep.insert(std::make_pair(525, " Key is not well-formed"));
} 

void Server::cmdPass(int clientFd, std::string data)
{
    
    if (data.empty())
    {
        std::stringstream err;
        err << ":" << inet_ntoa(this->serverAddr.sin_addr) << " " << 461 << " PASS " << this->errRep.find(461)->second << "\r\n";
        send(clientFd, err.str().c_str(), err.str().size(), 0);
    }
    else if (this->users.find(clientFd)->second.getIsConnected())
    {
        std::stringstream err;
        err << ":" << inet_ntoa(this->serverAddr.sin_addr) << " " << 462 << " PASS " << this->errRep.find(462)->second << "\r\n";
        send(clientFd, err.str().c_str(), err.str().size(), 0);
    }
    else if (!checkPass(data))
    {
        std::stringstream err;
        err << ":" << inet_ntoa(this->serverAddr.sin_addr) << " " << 464 << " PASS " << this->errRep.find(464)->second << "\r\n";
        send(clientFd, err.str().c_str(), err.str().size(), 0);
    }
    this->users.find(clientFd)->second.setSetPass(true);
    authenticate(clientFd);
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

void Server::authenticate(int clientFd)
{
    User u = this->users.find(clientFd)->second;
    if (!u.getNickName().empty() && u.getSetPass())
        this->users.find(clientFd)->second.setIsConected(true);
}

void Server::runCommand(int clientFd, std::string command)
{
    if (!command.empty())
    {
        std::stringstream ss(command);
        std::string cmdName, cmdParam;
        ss >> cmdName;
        ss >> cmdParam;
        if (Utils::stolower(cmdName) == "pass")
            cmdPass(clientFd, cmdParam);
        else if (Utils::stolower(cmdName) == "nick")
            cmdNick(clientFd, cmdParam);
        else if (Utils::stolower(cmdName) == "user")
            cmdUser(clientFd, cmdParam);
    }
    
}

