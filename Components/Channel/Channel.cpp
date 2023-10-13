#include "Channel.hpp"

/**************************************************************/
/*                        Constructors                        */
/**************************************************************/

Channel::Channel() : name("")
{
    this->key = "";
    this->topic = "";
    this->limit = 0;
    this->memberCount = 0;
    this->mode = 0;
}

Channel::Channel(std::string name, User *user) : name(name)
{
    this->addUser(user);
    this->key = "";
    this->topic = "";
    this->limit = 0;
    this->memberCount = 0;
    this->mode = 0;
}

Channel::Channel(std::string name) : name(name)
{

    this->name = name;
    this->limit = 0;
    this->memberCount = 0;
    this->key = "";
    this->topic = "";
    this->mode = 0;
}

Channel::Channel(const Channel &ch)
{
    if (this != &ch)
    {
        this->name = ch.name;
        this->key = ch.key;
        this->topic = ch.topic;
        this->users = ch.users;
        this->operators = ch.operators;
        this->invited = ch.invited;
        this->limit = ch.limit;
        this->memberCount = ch.memberCount;
        this->mode = ch.mode;
    }
}

/**************************************************************/
/*                    getters and setters                     */
/**************************************************************/
std::string Channel::getName() const { return this->name; }
void Channel::setName(std::string name) { this->name = name; }
std::string Channel::getKey() const { return this->key; }
void Channel::setKey(std::string key) { this->key = key; }
std::string Channel::getTopic() const { return this->topic.empty() ? "No topic is set" : this->topic; }
void Channel::setTopic(std::string topic) { this->topic = topic; }
const std::map<int, User *> &Channel::getUsers() const { return this->users; }
const std::map<int, User *> &Channel::getOperators() const { return this->operators; }
std::string Channel::getUserNickByFd(int fd) const { return this->users.at(fd)->getNickName(); }
int Channel::getLimit() const { return this->limit; }
void Channel::setLimit(int limit) { this->limit = limit; }
int Channel::getMemberCount() const { return this->memberCount; }
void Channel::setMemberCount(int memberCount) { this->memberCount = memberCount; }
int Channel::getMode() const { return this->mode; }
void Channel::setMode(int mode) { this->mode |= mode; }
void Channel::unsetMode(int mode) { this->mode &= ~mode; }

std::string Channel::getModeString() const
{
    std::string modeString;
    if (this->mode == 0)
        return modeString;
    modeString += "+";
    if (this->mode & Channel::INVITE_ONLY)
        modeString += "i";
    if (this->mode & Channel::MODERATED)
        modeString += "o";
    if (this->mode & Channel::TOPIC_PROTECTED)
        modeString += "t";
    if (this->mode & Channel::KEY)
        modeString += "k";
    if (this->mode & Channel::LIMIT)
        modeString += "l";
    return modeString;
}

/**************************************************************/
/*                         Functions                          */
/**************************************************************/
void Channel::addUser(User *user)
{

    if (this->operators.find(user->getClientFd()) != this->operators.end())
        return;
    if (this->users.find(user->getClientFd()) != this->users.end())
        return;
    if (!this->operators.size())
        addOperator(user);
    else
        this->users.insert(std::make_pair(user->getClientFd(), user));
    user->addChannel(this);
    std::string message = ":" + user->getNickName() + "!~" + user->getUserName() + "@" + user->getHostName() + " JOIN " + this->getName() + "\r\n";
    send(user->getClientFd(), message.c_str(), message.size(), 0);
    if (!this->getModeString().empty())
    {
        message = ":irc.leet.com 324 " + user->getNickName() + " " + this->name + " " + this->getModeString() + "\r\n";
        send(user->getClientFd(), message.c_str(), message.size(), 0);
    }
    message = ":irc.leet.com 332 " + user->getNickName() + " " + this->name + " :" + this->getTopic() + "\r\n";
    send(user->getClientFd(), message.c_str(), message.size(), 0);
    this->memberCount++;
    this->removeInvited(user->getNickName());
}

bool Channel::isInvited(std::string nickName)
{
    return std::find(this->invited.begin(), this->invited.end(), nickName) != this->invited.end();
}

void Channel::addInvited(std::string nickName)
{
    if (isInvited(nickName))
        return;
    this->invited.push_back(nickName);
}

void Channel::removeInvited(std::string nickName)
{
    if (!isInvited(nickName))
        return;
    this->invited.erase(std::find(this->invited.begin(), this->invited.end(), nickName));
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
        std::string message;
        message = ":irc.leet.com MODE " + this->name + " +o " + this->users.begin()->second->getNickName() + "\r\n";
        send(this->operators.begin()->first, message.c_str(), message.size(), 0);
        this->users.erase(this->users.begin());
    }
    this->memberCount--;
}

void Channel::broadcast(User *sender, std::string message, std::queue<std::pair<int, t_message> > *queue, bool all)
{
    uintptr_t receiverId, channelId = reinterpret_cast<uintptr_t>(this);
    message = ":" + sender->getNickName() + "!~" + sender->getUserName() + "@" + sender->getHostName() + " " + message;
    message += "\r\n";

    std::map<int, User *>::iterator it;
    if (all)
        for (it = this->users.begin(); it != this->users.end(); it++)
            if (it->second->getNickName() != sender->getNickName())
            {
                if (queue == NULL)
                {
                    send(it->first, message.c_str(), message.size(), 0);
                    continue;
                }
                receiverId = reinterpret_cast<uintptr_t>(it->second);
                t_message msg = {receiverId, channelId, this->name, message};
                queue->push(std::make_pair(it->first, msg));
            }
    for (it = this->operators.begin(); it != this->operators.end(); it++)
        if (it->second->getNickName() != sender->getNickName())
        {
            if (queue == NULL)
            {
                send(it->first, message.c_str(), message.size(), 0);
                continue;
            }
            receiverId = reinterpret_cast<uintptr_t>(it->second);
            t_message msg = {receiverId, channelId,this->name, message};
            queue->push(std::make_pair(it->first, msg));
        }
}

bool Channel::validateResponse(int receiverFd, t_message response)
{
    if (response.channelId != reinterpret_cast<uintptr_t>(this))
        return false;
    User *user = this->users.find(receiverFd) != this->users.end() ? this->users.find(receiverFd)->second : this->operators.find(receiverFd) != this->operators.end() ? this->operators.find(receiverFd)->second : NULL;
    if (!user)
        return false;
    if (reinterpret_cast<uintptr_t>(user) != response.receiverId)
        return false;
    return true;
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

void Channel::sendNames(int clientFd, std::string nickName)
{
    (void)clientFd;
    if (!this->users.size() && !this->operators.size())
        return;

    std::string message = ":irc.leet.com 353 " + nickName + " = " + this->name + " :";
    std::map<int, User *>::iterator it;
    if (this->operators.size())
        for (it = this->operators.begin(); it != this->operators.end(); it++)
            message += "@" + it->second->getNickName() + " ";
    if (this->users.size())
        for (it = this->users.begin(); it != this->users.end(); it++)
            message += it->second->getNickName() + " ";
    message += "\r\n";
    send(clientFd, message.c_str(), message.size(), 0);
    message = ":irc.leet.com 366 " + nickName + " = " + this->name + " :End of /NAMES list.\r\n";
    send(clientFd, message.c_str(), message.size(), 0);
}

bool Channel::isOperator(int clientFd)
{
    return this->operators.find(clientFd) != this->operators.end();
}

void Channel::changeOpMode(User *user, int mode)
{
    if (mode == 1)
    {
        this->operators.insert(std::make_pair(user->getClientFd(), user));
        this->users.erase(user->getClientFd());
    }
    else if (mode == 0)
    {
        this->users.insert(std::make_pair(user->getClientFd(), user));
        this->operators.erase(user->getClientFd());
    }
}

/**************************************************************/
/*                         Destructor                         */
/**************************************************************/
Channel::~Channel()
{
}