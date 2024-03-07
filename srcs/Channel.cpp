#include "Channel.hpp"

Channel::Channel() :
	topic("none")
{
	set_modes_false();
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
		name = other.name;
	}
	return *this;
}

Channel::Channel(std::string name, int op_fd) :
	topic("none"), name(name), op_fd(op_fd)
{
	set_modes_false();
}

void	Channel::set_modes_false(void)
{
	mode.i = false;
	mode.k = false;
	mode.l	= false;
	mode.o = false;
	mode.t = false;
}

void	Channel::add_client(Client* client)
{
	clients.push_back(client);
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

void	Channel::set_op(const int op_fd)
{
	this->op_fd = op_fd;
}

void	Channel::set_max_clients(int users)
{
	this->max_clients = users;
}

void	Channel::set_current_clients(int users)
{
	this->max_clients = users;
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

int	Channel::get_op(void) const
{
	return (this->op_fd);
}

std::string	Channel::get_password(void) const
{
	return (this->password);
}

int	Channel::get_max_clients(void) const
{
	return (this->max_clients);
}

int	Channel::get_current_clients(void) const
{
	return (this->current_clients);
}