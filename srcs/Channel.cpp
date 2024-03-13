#include "Channel.hpp"

Channel::Channel() :
	topic("")
{
	init_modes();
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

Channel::Channel(std::string name, Client *client) :
	topic(""), name(name), max_clients(0), current_clients(0), password("")
{
	init_modes();
	this->add_operator(client);
	add_client(client);
}

void	Channel::init_modes(void)
{
	mode.i = false;
	mode.k = false;
	mode.l	= false;
	mode.o = true;
	mode.t = true;
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

void Channel::send_message(Client *client, std::string message)
{
	std::vector<Client*>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it)
		if ((*it) != client)
			(*it)->send_message(message);
}

void	Channel::remove_client(Client* client)
{
	std::vector<Client*>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it) == client)
		{
			clients.erase(it);
			break;
		}
	}
	std::vector<int>::iterator fds;
	for (fds = operators_fds.begin(); fds != operators_fds.end(); ++fds)
	{
		if ((*fds) == client->fd)
		{
			operators_fds.erase(fds);
			break;
		}
	}
}

std::string	Channel::get_list_of_clients(Client *client)
{
	std::string list;
	std::vector<Client*>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if (*it == client)
			continue ;
		if (is_operator(*it))
			list += "@";
		else
			list += "+";
		list += (*it)->nickname;
		list += " ";
	}
	return (list);
}

std::string Channel::get_names_list(Client *client)
{
	std::string list;
	std::vector<Client*>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it)
	{
		if (*it == client)
			continue ;
		list += (*it)->nickname;
		list += " ";
	}
	return (list);

}

// Add and remove operators
void	Channel::add_operator(Client *client)
{
	operators_fds.push_back(client->fd);
}

void	Channel::remove_operator(Client *client)
{
	std::vector<int>::iterator it;
	for (it = operators_fds.begin(); it != operators_fds.end(); ++it)
	{
		if ((*it) == client->fd)
		{
			operators_fds.erase(it);
			break;
		}
	}
}

bool	Channel::is_operator(Client *client)
{
	std::vector<int>::iterator it;
	for (it = operators_fds.begin(); it != operators_fds.end(); ++it)
	{
		if ((*it) == client->fd)
			return (true);
	}
	return (false);
}

void	Channel::increase_clients(void)
{
	current_clients++;
}

void	Channel::decrease_clients(void)
{
	current_clients--;
	if (current_clients < 0)
		current_clients = 0;
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
		add_operator(client);
	else
		remove_operator(client);
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

void	Channel::set_creation_time(std::string creation_time)
{
	this->creation_time = creation_time;
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

std::string Channel::get_creation_time(void) const
{
	return (this->creation_time);
}

std::string Channel::get_mode_string(void)
{
	std::string mode = "+";
	if (this->mode.i == true)
		mode += "i";
	if (this->mode.t == true)
		mode += "t";
	if (this->mode.k == true)
		mode += "k";
	// if (this->mode.o == true)
	// 	mode += "o";
	if (this->mode.l == true)
		mode += "l";
	return (mode);
}