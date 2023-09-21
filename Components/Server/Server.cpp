/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmrabet <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 06:36:46 by zmrabet           #+#    #+#             */
/*   Updated: 2023/09/19 05:39:08 by zmrabet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
}

Server::Server(const Server& sv)
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

int Server::getServerSocket() const
{
    return this->serverSocket;
}

void Server::setServerSocket(int serverSocket)
{
    this->serverSocket = serverSocket;
}

struct sockaddr_in Server::getServerAddr() const
{
    return this->serverAddr;
}

void Server::setServerAddr(struct sockaddr_in serverAddr)
{
    this->serverAddr = serverAddr;
}

int Server::getSocketLen() const
{
    return this->socketLen;
}

void Server::setSocketLen(int socketLen)
{
    this->socketLen = socketLen;
}

unsigned int Server::getPort() const
{
    return this->port;
}

void Server::setPort(unsigned int port)
{
    this->port = port;
}

std::string Server::getPassword() const
{
    return this->password;
}

void Server::setPassword(std::string password)
{
    this->password = password;
}

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
    this->pfds.erase(this->pfds.begin() + indexClient);

    if (this->buffring.find(this->pfds[indexClient].fd) != this->buffring.end()) 
        this->buffring.erase(this->buffring.find(this->pfds[indexClient].fd));
    if (this->nickNames.find(this->pfds[indexClient].fd) != this->nickNames.end())
        this->nickNames.erase(this->nickNames.find(this->pfds[indexClient].fd));
    if (this->clients.find(this->pfds[indexClient].fd) != this->clients.end())
        this->clients.erase(this->clients.find(this->pfds[indexClient].fd));
}

void Server::joinBuffers(int indexClient, char *buffer)
{
    if (buffer[strlen(buffer) - 1] != '\n')
        this->buffring[this->pfds[indexClient].fd] += buffer;
    else
    {
        this->buffring[this->pfds[indexClient].fd] += buffer;
        std::cout << this->buffring[this->pfds[indexClient].fd];
        std::string command = this->buffring[this->pfds[indexClient].fd];
        this->buffring[this->pfds[indexClient].fd].clear();
        runCommand(this->pfds[indexClient].fd, command);
    }
}

void Server::requests(int indexClient)
{   
    if (this->pfds[indexClient].revents & (POLLHUP | POLL_ERR))
        clientDisconnected(indexClient);
    else if (this->pfds[indexClient].revents & POLLIN)
    {
        char buffer[1024];
        bzero(buffer, 1024);
        int r = recv(this->pfds[indexClient].fd, buffer, sizeof(buffer), 0);
        if (r <= 0)
            clientDisconnected(indexClient);
        else
            joinBuffers(indexClient, buffer);
    }
}

void Server::acceptClients()
{
    for (;;)
    {
        int numfds = poll(&(this->pfds[0]), this->pfds.size(), 0);
        if (numfds == -1)
            customException("Error : poll failed");
        if (numfds == 0)
            continue;
        if (this->pfds[0].revents & POLLIN)
        {
            struct sockaddr_in clientAddr;
            unsigned int csocketLen = sizeof(clientAddr);
            int client_fd = accept(this->serverSocket, (sockaddr *)&clientAddr, (socklen_t *)&csocketLen);
            std::cout << Utils::getTime() << " " << inet_ntoa(clientAddr.sin_addr) 
                <<" has joined the server."<<std::endl;
            if (client_fd == -1)
                customException("Error : accept failed");
            this->pfds.push_back((struct pollfd){.fd = client_fd, .events = POLLIN});
            this->clients.insert(std::make_pair(client_fd, Client(client_fd)));  
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
    acceptClients();
}


bool Server::checkDuplicateNick(std::string nickName)
{
    std::map<int, std::string>::iterator it;
    for (it = this->nickNames.begin(); it != this->nickNames.end(); it++)
    {
        if (it->second == nickName)
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

void Server::runCommand(size_t clientFd, std::string command)
{
    (void) clientFd;
    (void) command;
}