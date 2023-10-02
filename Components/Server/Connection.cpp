#include "Server.hpp"

/**************************************************************/
/*                         Functions                          */
/**************************************************************/

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
    if (this->nicks.find(this->users.at(clientFd).getNickName()) != this->nicks.end())
        this->nicks.erase(this->nicks.find(this->users.at(clientFd).getNickName()));
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
    if (this->pfds[indexClient].revents & (POLLHUP | POLLERR))
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
        std::cout << "buffer: "<< buffer << std::endl;
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
    struct sockaddr_in clientAddr;
    unsigned int csocketLen = sizeof(clientAddr);
    int client_fd = accept(this->serverSocket, (sockaddr *)&clientAddr, (socklen_t *)&csocketLen);
    if (client_fd == -1)
    {
        std::cerr << RED << "Error : accept failed (max_fd reached)" << DEFAULT << std::endl;
        return;
    }
    std::cout << Utils::getTime() << " " << inet_ntoa(clientAddr.sin_addr)
              << " has joined the server." << std::endl;
    this->pfds.push_back((struct pollfd){.fd = client_fd, .events = POLLIN | POLLOUT});
    this->users.insert(std::make_pair(client_fd, User(client_fd)));
    this->users[client_fd].setHostName(inet_ntoa(clientAddr.sin_addr));
    this->users[client_fd].joinChannel(&this->channels["general"]);
    if (client_fd == 4)
    {
        this->users[client_fd].joinChannel(&this->channels["data"]);
    }
}

void Server::multipleClients()
{
    for (;;)
    {
        int numfds = poll(&(this->pfds[0]), this->pfds.size(), POLL_TIMEOUT);
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