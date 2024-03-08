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
	topic("none"), name(name), op_fd(op_fd), max_clients(0), current_clients(0), password("")
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

std::string	Channel::handle_mode(std::vector<std::string> args)
{
	std::string mode_flags = args[1];
	bool mode = mode_flags[0] == '+';
	std::string modes_aplied = mode_flags.substr(0, 1);
	std::string modes_args;
	mode_flags = mode_flags.substr(1);
	args.erase(args.begin(), args.begin() + 2);
	std::vector<std::string>::iterator it;
	it = args.begin();
	for (size_t i = 0; i < mode_flags.size(); i++)
	{
		if (mode_flags[i] == 'i')
		{
			change_mode_i(mode);
			modes_aplied += "i";
		}
		if (mode_flags[i] == 't')
		{
			change_mode_t(mode);
			modes_aplied += "t";
		}
		if (mode_flags[i] == 'k')
		{
			if (it != args.end() && it->length() > 0)
			{
				change_mode_k(mode, args[0]);
				modes_aplied += "k";
				if (mode == true)
					modes_args += *(it);
				else
					modes_args += "*";
				it++;
			}
		}
		if (mode_flags[i] == 'o')
		{
			if (it != args.end() && it->length() > 0)
			{
				Client *client = find_client_by_nick(*it);
				if (client != NULL)
				{
					change_mode_o(mode, client);
					modes_aplied += "o";
					modes_args += *(it);
					it++;
				}
			}
		}
		if (mode_flags[i] == 'l')
		{
			if (it != args.end() && it->length() > 0)
			{
				int limit = std::stoi(*it);
				if ((mode == true && limit > 0) || mode == false)
				{
					modes_aplied += "l";
					modes_args += *(it);
					it++;
				}
			}
		}
	}
	std::string modes_change = modes_aplied + " " + modes_args;
	return (modes_change);
}

void	Channel::broadcast_message(std::string message)
{
	std::vector<Client*>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it)
		(*it)->send_message(message);
}

std::string	Channel::get_list_of_clients(void)
{
	std::string list;
	std::vector<Client*>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it)->fd == op_fd)
			list += "@";
		else
			list += "+";
		list += (*it)->nickname;
		list += " ";
	}
	return (list);
}

// Change modes	functions
void	Channel::change_mode_i(bool mode)
{
	this->mode.i = mode;
}

void	Channel::change_mode_t(bool mode)
{
	this->mode.t = mode;
}

void	Channel::change_mode_k(bool mode, std::string password)
{
	this->mode.k = mode;
	if (mode == true)
		this->password = password;
	else
		this->password = "";
}

void	Channel::change_mode_o(bool mode, Client *client)
{
	this->mode.o = mode;
	if (mode == true)
		this->op_fd = client->fd;
	else
		this->op_fd = 0;
}

void	Channel::change_mode_l(bool mode, int limit)
{
	this->mode.l = mode;
	if (mode == true)
		this->max_clients = limit;
	else
		this->max_clients = 0;
}

Client*	Channel::find_client_by_nick(std::string nick)
{
	std::vector<Client*>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it)->nickname == nick)
			return (*it);
	}
	return (NULL);
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