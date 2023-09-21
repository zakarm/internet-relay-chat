
#include "Components/Server/Server.hpp"

int main(int argc, char *argv[])
{
    
    if (argc == 3)
    {
        try
        {
            unsigned int port = atoi(argv[1]);
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