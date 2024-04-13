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

bool	Channel::handle_mode(bool status, char mode, std::string arg, Client *client)
{
	switch (mode)
	{
		case 'i': // Invite only
			return (change_mode_i(status));
		case 't': // Topic
			return (change_mode_t(status));
		case 'k': // Key
			return (change_mode_k(status, arg));
		case 'l': // Limit
			try
			{
				change_mode_l(status, std::stoi(arg));
				return true;
			}
			catch(const std::exception& e)
			{
				return (false);
			}
			
		case 'o': // Operator
			return (change_mode_o(status, arg, client));
		default:
			return (false);
	}
}
//TODO
std::string	Channel::check_modes(std::vector<std::string> args, Client *client)
{
	bool	mode_status;
	bool	put_sign;
	std::string	valid_flags;
	std::string	valid_args;
	std::string arg;
	std::string	mode_flags = args[1];
	std::vector<std::string> mode_args = split(join_split(args, 2, " "), ' ');
	if (mode_args.size() == 0)
		mode_args.push_back("");
	std::vector<std::string>::iterator mode_it;
	mode_it = mode_args.begin();
	std::vector<char>::iterator it;
	for (it = mode_flags.begin(); it != mode_flags.end(); ++it)
	{
		if (*it == '+' || *it == '-')
		{
			if (it + 1 != mode_flags.end())
				if ((*(it + 1) == '+' || *(it + 1) == '-') && mode_it != mode_args.end())
					continue;
			put_sign = true;
			mode_status = (*it == '+')? true : false;
		}
		if (mode_it == mode_args.end())
			arg = "";
		else
			arg = *mode_it;
		if (handle_mode(mode_status, *it, arg, client))
		{
			if (put_sign == true)
			{
				put_sign = false;
				if (mode_status == true)
					valid_flags += "+";
				else
					valid_flags += "-";
			}
			valid_flags += *it;
			if (*it == 'k' && mode_status == false)
				valid_args += "*";
			else if (*it == 'k' || *it == 'l' || *it == 'o')
				valid_args += arg + " ";
			if (*it == 'k' || *it == 'l' || *it == 'o')
				if (mode_it != mode_args.end())
					mode_it++;
		}
	}
	return (valid_flags + " " + valid_args);
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
	decrease_clients();
}

std::string	Channel::get_list_of_clients(Client *client)
{
	(void)client;
	std::string list;
	std::vector<Client*>::iterator it;
	for (it = clients.begin(); it != clients.end(); ++it)
	{
		// if (*it == client)
		// 	continue ;
		if (is_operator(*it))
			list += "@";
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
bool	Channel::change_mode_i(bool mode)
{
	if (this->mode.i == mode)
		return (false);
	this->mode.i = mode;
	return (true);
}

bool	Channel::change_mode_t(bool mode)
{
	if (this->mode.t == mode)
		return (false);
	this->mode.t = mode;
	return (true);
}

bool	Channel::change_mode_k(bool mode, std::string password)
{
	if (password.length() == 0)
		return (false);
	else
	if (mode == false)
	{
		this->mode.k = mode;
		this->password = "";
	}
	else if (mode == true)
	{
		this->mode.k = mode;
		this->password = password;
	}
	return (true);
}

bool	Channel::change_mode_o(bool mode, std::string name_op, Client *client)
{
	Client *op = find_client_by_nick(name_op);
	if (op == NULL)
	{
		client->send_message(ERR_NOSUCHNICK(client->get_realname(), name_op));
		return (false);
	}
	if (!is_operator(client))
	{
		client->send_message(ERR_CHANOPRIVSNEEDED(client->get_realname(), this->name));
		return (false);
	}
	if (mode == true)
		add_operator(op);
	else if (mode == false)
		remove_operator(op);
	return (true);
}

bool	Channel::change_mode_l(bool mode, int limit)
{
	if ((this->mode.l == false && mode == false) || limit <= 0)
		return (false);
	if (mode == false)
	{
		this->mode.l = mode;
		this->max_clients = 0;
	}
	else if (mode == true)
	{
		this->mode.l = mode;
		this->max_clients = limit;
	}
	return (true);
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
	increase_clients();
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

std::vector<int> Channel::get_operators_fds() const
{
	return operators_fds;
}