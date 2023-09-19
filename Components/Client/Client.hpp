/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmrabet <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/14 01:43:11 by zmrabet           #+#    #+#             */
/*   Updated: 2023/09/19 02:08:34 by zmrabet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "../Components.hpp"

class Client
{
    private :
        int clientFd;
        std::string nickName;
        std::string userName;
        std::string realName;
        
    public :
        Client(int clientFd);
        Client(int clientFd, std::string nickName, std::string userName, std::string realName);
        std::string getNickName() const;
        void setNickName(std::string nickName);

        std::string getUserName() const;
        void setUserName(std::string userName);

        std::string getRealName() const;
        void setRealName(std::string realName);

        int getClientFd() const;
        void setClientFd(int clientFd);
};

#endif