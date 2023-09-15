/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmrabet <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/14 01:43:11 by zmrabet           #+#    #+#             */
/*   Updated: 2023/09/15 02:21:05 by zmrabet          ###   ########.fr       */
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
        std::string getNickName() const;
        void setNickName(std::string nickName);

        std::string getUserName() const;
        void setUserName(std::string userName);

        std::string getRealName() const;
        void setRealName(std::string realName);
};

#endif