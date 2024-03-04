#include "Channel.hpp"

Channel::Channel() :
	mode{false, false, false, false, false}, topic("none")
{

}

Channel::~Channel()
{

}

Channel::Channel(const Channel& copy)
{
	*this = copy;
}

Channel& Channel::operator=(const Channel& other)
{
	if (this != &other)
	{
		mode = other.mode;
		topic = other.topic;
	}
	return *this;
}

Channel::Channel(std::string name) :
	mode{false, false, false, false, false}, name(name), topic("none")
{

}

void	Channel::set_topic(const std::string topic)
{
	this->topic = topic;
}

void	Channel::set_name(const std::string name)
{
	this->name = name;
}

void	Channel::set_mode(const t_channel_modes mode)
{
	this->mode = mode;
}

std::string	Channel::get_topic(void) const
{
	return (this->topic);
}

std::string Channel::get_name(void) const
{
	return (this->name);
}

t_channel_modes	Channel::get_mode(void) const
{
	return (this->mode);
}