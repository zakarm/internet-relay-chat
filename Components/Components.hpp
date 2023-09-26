#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#ifndef RED
#define RED "\033[31m"
#endif

#ifndef DEFAULT
#define DEFAULT "\033[39m"
#endif

#define POLL_TIMEOUT 10

#ifdef DEBUG_MODE
# define DEBUG_MSG(msg) std::cout << "Debug: " << msg
#else
# define DEBUG_MSG(msg)
#endif

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <poll.h>
#include <vector>
#include <fcntl.h>
#include <sys/types.h>
#include <sstream>
#include <arpa/inet.h>
#include <map>
#include <queue>
#include <signal.h>

#endif