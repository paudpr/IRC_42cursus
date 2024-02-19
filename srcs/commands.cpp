#include "ft_irc.hpp"

void Server::pass(const int& fd, Message& message)
{
	(void)fd;
	(void)message;
	std::cout << PINK << "Command password" << RESET << std::endl;
}

bool Server::check_availability(std::string& nick, std::string& client_nick)
{
	if (client_nick.empty())
		return true;
	std::string aux(nick);
	to_lower(aux);
	for (std::vector<Client*>::iterator iter = clients.begin(); iter != clients.end(); iter++)
	{
		std::string str((*iter)->nickname);
		to_lower(str);
		if (str == aux)
			return false;
	}
	return true;
}

void Server::nick(const int& fd, Message& message)
{
	std::vector<Client*>::iterator client = get_client_byfd(fd);
	if ((*client)->is_online == false)
		return  send_message(fd, "CLIENT not connected, can't change nick");
	if (message.args.empty())
		return send_message(fd, ERR_NONICKNAMEGIVEN((*client)->get_realname()));
	if (message.args.size() > 1 
		|| message.args[0][0] == '$' || message.args[0][0] == ':'
		|| message.args[0][0] == '&' || message.args[0][0] == '#'
		|| message.args[0].find(" ,*?!@.") != std::string::npos)
		return send_message(fd, "ERROR nick erróneo");
	if (!check_availability(message.args[0], (*client)->nickname))
		return send_message(fd, "ERROR nick ya en uso");

	std::string prev = (*client)->nickname;
	(*client)->nickname = message.args[0];

	//change nickname in all channels with necessary messages
	std::string msg  = ":" + prev + "!~" /*+ get_hostname() */+ " NICK " + (*client)->nickname; 
	send_message(fd, msg);
	check_valid_user(*client);
}

void Server::user(const int& fd, Message& message)
{
	std::vector<Client*>::iterator client = get_client_byfd(fd);
	if ((*client)->is_online == false)
		return  send_message(fd, "CLIENT not connected, can't change user");
	if (message.args.size() < 4)
		return send_message(fd, ERR_NEEDMOREPARAMS((*client)->get_realname()));
	if ((*client)->username.empty() == false)
		return send_message(fd, ERR_ALREADYREGISTERED((*client)->get_realname()));
	(*client)->username = message.args[0];
	(*client)->hostname = inet_ntoa(conn_addr.sin_addr);
	if (message.args.size() >= 4)
		(*client)->realname = join_split(message.args, 3);
	if ((*client)->realname[0] == ':')
		(*client)->realname.erase(0, 1);
	check_valid_user(*client);
}

void send_welcome(Client *client)
{
	(void)  client;
	std::cout << PINK << "enviar mensajes de  ahberse  podido conectar. solo pasa una vez" << std::endl;
	std::cout << "[001]" << std::endl;
	std::cout << "[002]" << std::endl;
	std::cout << "[003]" << std::endl;
	std::cout << "[004]" << RESET << std::endl;
	//no  son prints, con enviar mensajes, pero lo que sea
}

bool Server::check_valid_user(Client *client)
{
	if (client->nickname.empty() || client->username.empty() || (client->mode & VALID_CLIENT))
		return false;
	if (!client->nickname.empty() && !client->username.empty() && !(client->mode & VALID_CLIENT))
	{
		client->mode |= VALID_CLIENT;
		client->ping_request = false;
		send_welcome(client);
	}
	return true;
}

void Server::whois(const int& fd, Message& message)
{
	(void)fd;
	(void)message;
}

void Server::quit(const int& fd, Message& message)
{
	std::string msg = join_split(message.args, 0);
	msg = "QUIT: " + msg;
	std::cout << "COSAS " <<  std::endl;
	std::cout << YELLOW << msg << RESET << std::endl;
	send_message(fd, msg);

	std::vector<pollfd>::iterator iter;
	for (iter = fds_poll.begin(); iter != fds_poll.end(); iter++)
	{
		if (iter->fd == fd)
			break ;
	}
	remove_client(iter);
	//abandonar channel y notificar a usuarios en el canal


}