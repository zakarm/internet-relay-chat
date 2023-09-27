#include "Channel.hpp"

/**************************************************************/
/*                        Constructors                        */
/**************************************************************/

Channel::Channel() : name("")
{
    this->key = "";
    this->topic = "";
    this->mode = 0;
}

Channel::Channel(std::string name, User *user) : name(name)
{
    this->operators.insert(std::make_pair(user->getClientFd(), user));
    this->key = "";
    this->topic = "";
    this->mode = 0;
}

Channel::Channel(std::string name) : name(name)
{

    this->key = "";
    this->topic = "";
    this->mode = NO_MODE;
}

Channel::Channel(const Channel &ch)
{
    if (this != &ch)
        this->name = ch.name;
}

/**************************************************************/
/*                    getters and setters                     */
/**************************************************************/
std::string Channel::getName() const { return this->name; }
void Channel::setName(std::string name) { this->name = name; }
std::string Channel::getKey() const { return this->key; }
void Channel::setKey(std::string key) { this->key = key; }
std::string Channel::getTopic() const { return this->topic; }
void Channel::setTopic(std::string topic) { this->topic = topic; }
const std::map<int, User *>& Channel::getUsers() const { return this->users; }
const std::map<int, User *>& Channel::getOperators() const { return this->operators;}
std::string Channel::getUserNickByFd(int fd) const { return this->users.at(fd)->getNickName(); }

/**************************************************************/
/*                         Functions                          */
/**************************************************************/
void Channel::addUser(User *user)
{
    if (!this->operators.size())
    {
        addOperator(user);
        return;
    }
    if (this->users.find(user->getClientFd()) != this->users.end())
        return;
    this->users.insert(std::make_pair(user->getClientFd(), user));
}

void Channel::addOperator(User *user)
{
    if (this->operators.find(user->getClientFd()) != this->operators.end())
        return;
    this->operators.insert(std::make_pair(user->getClientFd(), user));
}

void Channel::removeUser(int clientFd)
{
    if (this->users.find(clientFd) != this->users.end())
        this->users.erase(clientFd);
    if (this->operators.find(clientFd) != this->operators.end())
        this->operators.erase(clientFd);
    if (!this->operators.size() && this->users.size())
    {
        this->operators.insert(std::make_pair(this->users.begin()->first, this->users.begin()->second));
        this->users.erase(this->users.begin());
    }
}

void Channel::sendToAll(std::string sender, std::string message, bool all)
{
    message = ":" + sender + " PRIVMSG #" + this->name + " :" + message + "\r\n";

    std::map<int, User *>::iterator it;
    if (all)
    {
        for (it = this->users.begin(); it != this->users.end(); it++)
            if (it->second->getNickName() != sender)
                send(it->first, message.c_str(), message.size(), 0);
    }
    for (it = this->operators.begin(); it != this->operators.end(); it++)
        if (it->second->getNickName() != sender)
            send(it->first, message.c_str(), message.size(), 0);
}

int Channel::getMode() const
{
    return this->mode;
}

void Channel::setMode(int mode)
{
    this->mode |= mode;
}

void Channel::unsetMode(int mode)
{
    this->mode &= ~mode;
}

void Channel::listUsers()
{
    std::cout << "Channel name: " << this->name << std::endl;
    std::map<int, User *>::iterator it;
    std::cout << "\tOperators:" << std::endl;
    for (it = this->operators.begin(); it != this->operators.end(); it++)
        std::cout << "\t\t" << it->second->getNickName() << std::endl;
    std::cout << "\tUsers:" << std::endl;
    for (it = this->users.begin(); it != this->users.end(); it++)
        std::cout << "\t\t" << it->second->getNickName() << std::endl;
}


bool Channel::isOperator(int clientFd)
{
    return this->operators.find(clientFd) != this->operators.end();
}

/**************************************************************/
/*                         Destructor                         */
/**************************************************************/
Channel::~Channel()
{
}