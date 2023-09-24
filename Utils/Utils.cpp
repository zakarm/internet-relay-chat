/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmrabet <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/14 23:37:41 by zmrabet           #+#    #+#             */
/*   Updated: 2023/09/14 23:48:01 by zmrabet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

std::string Utils::getTime()
{
    std::stringstream timeStream;
    time_t now = time(NULL);
    struct tm *tm_struct = localtime(&now);

    int hour = tm_struct->tm_hour;
    int min = tm_struct->tm_min;
    timeStream << RED << "[" << hour << ":" << min << "]" << DEFAULT;
    return timeStream.str();
}

std::string Utils::getDate()
{
    std::time_t now = std::time(NULL);
    std::tm *ptm = std::localtime(&now);
    return std::asctime(ptm);
}

std::string Utils::stolower(std::string data)
{
    for (size_t i = 0; i < data.size(); i++)
    {
        if (isalpha(data[i]))
            data[i] = std::tolower(data[i]);
    }
    return data;
}

void Utils::loadErrorsReplies(std::map<int, std::string> &errRep, std::string netName, 
    std::string servName, std::string hostName)
{

    errRep.insert(std::make_pair(1, ":Welcome to the " + netName + " Network,"));
    errRep.insert(std::make_pair(2, ":Your host is " + servName + ", running version"));
    errRep.insert(std::make_pair(3, ":This server was created"));
    errRep.insert(std::make_pair(4, ":" + servName + " v1"));
    errRep.insert(std::make_pair(5,":are supported by this server"));
    errRep.insert(std::make_pair(10, hostName));
    errRep.insert(std::make_pair(212,""));
    errRep.insert(std::make_pair(219,""));
    errRep.insert(std::make_pair(242,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));
    // errRep.insert(std::make_pair(,""));

    
    errRep.insert(std::make_pair(401, " No such nick/channel"));
    errRep.insert(std::make_pair(402, " No such server"));
    errRep.insert(std::make_pair(403, " No such channel"));
    errRep.insert(std::make_pair(404, " Cannot send to channel"));
    errRep.insert(std::make_pair(405, " You have joined too many channels"));
    errRep.insert(std::make_pair(406, " There was no such nickname"));
    errRep.insert(std::make_pair(409, " No origin specified"));
    errRep.insert(std::make_pair(411, " No recipient given"));
    errRep.insert(std::make_pair(412, " No text to send"));
    errRep.insert(std::make_pair(417, " Input line was too long"));
    errRep.insert(std::make_pair(421, " Unknown command"));
    errRep.insert(std::make_pair(422, " MOTD File is missing"));
    errRep.insert(std::make_pair(431, " No nickname given"));
    errRep.insert(std::make_pair(432, " Erroneus nickname"));
    errRep.insert(std::make_pair(433, " Nickname is already in use"));
    errRep.insert(std::make_pair(436, " Nickname collision KILL from"));
    errRep.insert(std::make_pair(441, " They aren't on that channel"));
    errRep.insert(std::make_pair(442, " You're not on that channel"));
    errRep.insert(std::make_pair(443, " is already on channel"));
    errRep.insert(std::make_pair(451, " You have not registered"));
    errRep.insert(std::make_pair(461, " Not enough parameters."));
    errRep.insert(std::make_pair(462, " You may not reregister."));
    errRep.insert(std::make_pair(464, " Password incorrect"));
    errRep.insert(std::make_pair(465, " You are banned from this server."));
    errRep.insert(std::make_pair(471, " Cannot join channel (+l)"));
    errRep.insert(std::make_pair(472, " is unknown mode char to me"));
    errRep.insert(std::make_pair(473, " Cannot join channel (+i)"));
    errRep.insert(std::make_pair(474, " Cannot join channel (+b)"));
    errRep.insert(std::make_pair(475, " Cannot join channel (+k)"));
    errRep.insert(std::make_pair(476, " Bad Channel Mask"));
    errRep.insert(std::make_pair(481, " Permission Denied- You're not an IRC operator"));
    errRep.insert(std::make_pair(482, " You're not channel operator"));
    errRep.insert(std::make_pair(483, " You cant kill a server!"));
    errRep.insert(std::make_pair(491, " No O-lines for your host"));
    errRep.insert(std::make_pair(501, " Unknown MODE flag"));
    errRep.insert(std::make_pair(502, " Cant change mode for other users"));
    errRep.insert(std::make_pair(524, " No help available on this topic"));
    errRep.insert(std::make_pair(525, " Key is not well-formed"));
} 