
#include "Components/Server/Server.hpp"


int main(int argc, char *argv[])
{ 
    if (argc == 3)
    {
        try
        {
            port_check(argv[1]);
            unsigned int port = atoi(argv[1]); // use iostream to check if it's a number
            std::string password = argv[2]; 
            Server s(port, password);
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