#include "Bot.hpp"

int main (int argc, char **argv)
{
    try
    {
        if (argc != 4)
            exit(0);
        Bot bot(atoi(argv[1]), argv[2], argv[3]);
    }
    catch(std::exception &ex)
    {
        std::cerr << RED << "Error : " << ex.what() << DEFAULT << std::endl;
    }
}