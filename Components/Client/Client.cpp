/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmrabet <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/14 01:43:07 by zmrabet           #+#    #+#             */
/*   Updated: 2023/09/19 02:10:26 by zmrabet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/**************************************************************/
/*                        Constructors                        */
/**************************************************************/

Client::Client(int clientFd) : clientFd(clientFd)
{
    
}

Client::Client(int clientFd, std::string nickName, std::string userName, std::string realName) :
    clientFd(clientFd), nickName(nickName), userName(userName), realName(realName)
{
    
}

/**************************************************************/
/*                    getters and setters                     */
/**************************************************************/

std::string Client::getNickName() const
{
    return this->nickName;
}

void Client::setNickName(std::string nickName)
{
    this->nickName = nickName;   
}

std::string Client::getUserName() const
{
    return this->userName;
}

void Client::setUserName(std::string userName)
{
    this->userName = userName;
}

std::string Client::getRealName() const
{
    return this->realName; 
}

void Client::setRealName(std::string realName)
{
    this->realName = realName;
}

int Client::getClientFd() const
{
    return this->clientFd;
}

void Client::setClientFd(int clientFd)
{
    this->clientFd = clientFd;
}
/**************************************************************/
/*                         Functions                          */
/**************************************************************/
