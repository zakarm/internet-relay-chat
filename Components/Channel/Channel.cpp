#include "Channel.hpp"

/**************************************************************/
/*                        Constructors                        */
/**************************************************************/
Channel::Channel(std::string name, User &user) : name(name)
{
    this->operators.insert(std::make_pair(user.getClientFd(), user));
    this->key = "";
    this->topic = "";
    this->mode = 0;
}

/**************************************************************/
/*                    getters and setters                     */
/**************************************************************/
std::string Channel::getName() const
{
    return this->name;
}

/**************************************************************/
/*                         Functions                          */
/**************************************************************/
void Channel::addUser(User &user)
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

void Channel::setMode(int mode){this->mode |= mode;}
void Channel::unsetMode(int mode) {this->mode &= ~mode;}

/**************************************************************/
/*                         Destructor                         */
/**************************************************************/
Channel::~Channel()
{
}