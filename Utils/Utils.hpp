#ifndef UTILS_HPP
#define UTILS_HPP
#include <time.h>
#include <iostream>
#include <sstream>
#include <map>

#ifndef RED
#define RED "\033[31m"
#endif

#ifndef DEFAULT
#define DEFAULT "\033[39m"
#endif

class Utils {
    private :
        Utils();
    public :
        static std::string getTime();
        static std::string stolower(std::string data);
        static void loadErrorsReplies(std::map<int, std::string> &errRep, std::string netName, 
            std::string servName, std::string hostName);
        static std::string getDate();
        int countWords(std::string data);
};

#endif //UTILS_HPP
