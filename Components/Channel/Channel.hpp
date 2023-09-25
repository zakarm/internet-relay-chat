#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include "../Components.hpp"
#include "../User/User.hpp"

class User; 

class Channel{
    private :
        std::string name;
        std::string key;
        std::string topic;
        std::map<int, User*> users;
        std::map<int, User*> operators;
        std::vector<std::string> banList;
        int limit;
        int mode;

    public :
        Channel();
        Channel(std::string name, User *user);
        Channel(std::string name);
        Channel(const Channel &channel);
        ~Channel();
        

        std::string getName() const;
        void setName(std::string name);
        std::string getKey() const;
        void setKey(std::string key);
        std::string getTopic() const;
        void setTopic(std::string topic);

        std::string getUserNickByFd(int fd) const;
        std::map<int, User*> getUsers() const;
        std::map<int, User*> getOperators() const;
        std::vector<std::string> getBanList() const;
        int getLimit() const;
        
        int getMode() const;
        void setMode(int mode);


        bool isOperator(int clientFd);
        void unsetMode(int mode);
        void addUser(User *user);
        void addOperator(User *user);
        void removeUser(int clientFd);
        enum MODES{
            NO_MODE = 0,
            INVITE_ONLY = 1,
            TOPIC_PROTECTED = 2,
            SECRET = 4,
            MODERATED = 8,
            LIMIT = 16,
            KEY = 32,
        };
        void listUsers();

};
#endif //CHANNEL_HPP
