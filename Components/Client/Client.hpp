/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmrabet <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/14 01:43:11 by zmrabet           #+#    #+#             */
/*   Updated: 2023/09/16 08:04:34 by zmrabet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "../Components.hpp"

class Client
{
    private :
        std::string nickName;
        std::string userName;
        std::string realName;
        
    public :
        Client(std::string nickName, std::string userName, std::string realName);
        std::string getNickName() const;
        void setNickName(std::string nickName);

        std::string getUserName() const;
        void setUserName(std::string userName);

        std::string getRealName() const;
        void setRealName(std::string realName);
};

#endif