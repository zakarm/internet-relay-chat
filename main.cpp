/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmrabet <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 06:36:25 by zmrabet           #+#    #+#             */
/*   Updated: 2023/09/13 06:36:26 by zmrabet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server/Server.hpp"

int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        try
        {
            unsigned int port = atoi(argv[1]);
            std::string password = argv[2];
            std::cout << port << password << std::endl;
            Server s(port);
            s.runServer();
        }
        catch (std::exception &ex)
        {
            std::cout << RED << ex.what() << DEFAULT << std::endl;
        }
    }
    else
        std::cout << RED << "Error : invalid args" << DEFAULT << std::endl;
}