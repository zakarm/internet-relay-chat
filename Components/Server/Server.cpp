/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmrabet <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 06:36:46 by zmrabet           #+#    #+#             */
/*   Updated: 2023/09/18 22:59:08 by zmrabet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/**************************************************************/
/*                        Constructors                        */
/**************************************************************/

Server::Server(int port) : port(port)
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

void Server::requests(int indexClient)
{
    if (this->pfds[indexClient].revents & POLLIN)
    {
        char buffer[1024];
        bzero(buffer, 1024);
        int r = recv(this->pfds[indexClient].fd, buffer, sizeof(buffer), 0);
        if (r == 0)
        {
            close(this->pfds[indexClient].fd);
            this->pfds.erase(this->pfds.begin() + indexClient);
            std::cout << "Client : " << indexClient << " has left the server." << std::endl;
        }
        else
        {
            std::cout << buffer;
            
            if (std::strcmp(buffer, "exit\n") == 0)
                {
                    close(this->pfds[indexClient].fd);
                    this->pfds.erase(this->pfds.begin() + indexClient);
                    std::cout << "Client : " << indexClient << " has left the server." << std::endl;
                }
        }
        send(this->pfds[indexClient].fd, "reseaved\n", strlen("reseaved\n"), 0);
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
                      << " has joined the server." << std::endl;
            if (client_fd == -1)
                customException("Error : accept failed");
            this->pfds.push_back((struct pollfd){.fd = client_fd, .events = POLLIN});
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

void Server::loginClient(size_t indexClient)
{
    if (indexClient + 1 >= this->clients.size())
    {
        this->clients.push_back(Client(this->pfds[indexClient].fd, "", "", ""));
    }
}

void Server::runCommand(size_t indexClient, std::string command)
{
    // loginClient(indexClient);
    (void)indexClient;
    (void)command;
}