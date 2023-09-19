/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmrabet <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 06:36:35 by zmrabet           #+#    #+#             */
/*   Updated: 2023/09/19 05:04:42 by zmrabet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include "../Components.hpp"
#include "../../Utils/Utils.hpp"
#include "../Client/Client.hpp"

class Server{
    private :
        int serverSocket;
        struct sockaddr_in serverAddr;
        int socketLen;
        unsigned int port;
        std::vector<struct pollfd> pfds;
        std::vector<Client> clients;

    public :
        Server(int port);
        Server(const Server& sv);
        int getServerSocket() const;
        void setServerSocket(int serverSocket);

        struct sockaddr_in getServerAddr() const;
        void setServerAddr(struct sockaddr_in serverAddr);

        int getSocketLen() const;
        void setSocketLen(int socketLen);

        unsigned int getPort() const;
        void setPort(unsigned int port);

        void customException(std::string errorMessage);
        void noBlockingFd();
        void socketOptions();
        void bindServer();
        void listenServer();
        void acceptClients();
        void requests(int indexClient);
        void runServer();

        void runCommand(size_t clientFd, std::string command);
};

#endif