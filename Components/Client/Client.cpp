/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmrabet <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/14 01:43:07 by zmrabet           #+#    #+#             */
/*   Updated: 2023/09/16 08:04:40 by zmrabet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/**************************************************************/
/*                        Constructors                        */
/**************************************************************/

Client::Client(std::string nickName, std::string userName, std::string realName)
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

/**************************************************************/
/*                         Functions                          */
/**************************************************************/
