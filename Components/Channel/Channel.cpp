#include "Channel.hpp"


void Channel::setMode(int mode){this->mode |= mode;}
void Channel::unsetMode(int mode) {this->mode &= ~mode;}

Channel::Channel(std::string name, User &user) : name(name)
{
    this->users.push_back(user);
    this->key = "";
    this->topic = "";
    this->mode = 0;
}