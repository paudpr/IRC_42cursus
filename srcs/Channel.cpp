#include "Channel.hpp"

Channel::Channel() :
	mode{false, false, false, false, false}, topic("default")
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

Channel::Channel(std::string topic) :
	mode{false, false, false, false, false}, topic(topic)
{

}