#include "Channel.hpp"

/**************************************************************/
/*                        Constructors                        */
/**************************************************************/

Channel::Channel(std::string name, User user) : name(name)
{
    this->operators.insert(std::make_pair(user.getClientFd(), user));
    this->key = "";
    this->topic = "";
    this->mode = 0;
}

Channel::Channel(const Channel& ch) 
{
    if (this != &ch)
        this->name = ch.name;
}

/**************************************************************/
/*                    getters and setters                     */
/**************************************************************/
std::string Channel::getName() const{ return this->name;}
void Channel::setName(std::string name){ this->name = name;}
std::string Channel::getKey() const{ return this->key;}
void Channel::setKey(std::string key){ this->key = key;}
std::string Channel::getTopic() const{ return this->topic;}
void Channel::setTopic(std::string topic){ this->topic = topic;}
std::map<int, User> Channel::getUsers() const{ return this->users;}

/**************************************************************/
/*                         Functions                          */
/**************************************************************/
void Channel::addUser(User user)
{
    if (this->users.find(user.getClientFd()) != this->users.end())
        return;
    this->users.insert(std::make_pair(user.getClientFd(), user));
}

void Channel::removeUser(int clientFd)
{
    this->users.erase(clientFd);
    this->operators.erase(clientFd);
}

void Channel::setMode(int mode)
{
    this->mode |= mode;
}

void Channel::unsetMode(int mode) 
{
    this->mode &= ~mode;
}

/**************************************************************/
/*                         Destructor                         */
/**************************************************************/
Channel::~Channel()
{
    
}