
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
        std::map<int, User> users;
        std::map<int, User> operators;
        std::vector<std::string> banList;
        int limit;
        int mode;
    public :
        Channel(std::string name, User &user);
        Channel(const Channel &channel);
        ~Channel();
        
        void setMode(int mode);
        void unsetMode(int mode);
        
        /// Getters
        std::string getName() const;
        std::string getKey() const;
        std::string getTopic() const;
        std::vector<User> getUsers() const;
        std::vector<std::string> getBanList() const;
        int getLimit() const;
        int getMode() const;

        /// Setters
        void setName(std::string name);
        void setKey(std::string key);
        void setTopic(std::string topic);


        void addUser(User &user);
        void removeUser(int clientFd);

};
#endif //CHANNEL_HPP
