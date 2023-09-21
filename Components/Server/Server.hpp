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
        std::string password;
        std::vector<struct pollfd> pfds;
        std::map<int, std::string> buffring;
        std::map<int, std::string> nickNames;
        std::map<int, Client> clients;

    public :
        Server(int port, std::string password);
        Server(const Server& sv);
        int getServerSocket() const;
        void setServerSocket(int serverSocket);
        struct sockaddr_in getServerAddr() const;
        void setServerAddr(struct sockaddr_in serverAddr);
        int getSocketLen() const;
        void setSocketLen(int socketLen);
        unsigned int getPort() const;
        void setPort(unsigned int port);
        std::string getPassword() const;
        void setPassword(std::string password);

        void customException(std::string errorMessage);
        void noBlockingFd();
        void socketOptions();
        void bindServer();
        void listenServer();
        void acceptClients();
        void clientDisconnected(int indexClient);
        void joinBuffers(int indexClient, char *buffer);
        void requests(int indexClient);
        void runServer();

        bool checkPass(std::string password);
        bool checkDuplicateNick(std::string nickName);
        void runCommand(size_t clientFd, std::string command);
};

#endif