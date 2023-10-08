
#include "Components/Server/Server.hpp"

void sig_handler(int signum)
{
    if (signum == SIGINT)
    {
        std::cout << std::endl << Utils::getTime() << " CIH: SERVER TAYE7" << std::endl;
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[])
{ 
    if (argc == 3)
    {
        signal(SIGINT, sig_handler);
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
