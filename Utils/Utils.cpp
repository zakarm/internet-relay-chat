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