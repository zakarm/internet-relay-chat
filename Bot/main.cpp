#include "Bot.hpp"

int main (int argc, char **argv)
{
    try
    {
        if (argc != 4)
        {
            std::cerr << RED << "Error: Invalid args" << DEFAULT << std::endl;
            exit(0);
        }
        port_check(argv[1]);
        Bot bot(atoi(argv[1]), argv[2], argv[3]);
    }
    catch(std::exception &ex)
    {
        std::cerr << RED << ex.what() << DEFAULT << std::endl;
    }
}