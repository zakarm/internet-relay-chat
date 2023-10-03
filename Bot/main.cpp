#include "bot.hpp"

int main (int argc, char **argv)
{
    if (argc != 4)
        exit(0);
    Bot bot(atoi(argv[1]), argv[2], argv[3]);
    return 0;
}