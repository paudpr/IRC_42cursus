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
	for (std::vector<User>::iterator iter = clients.begin(); iter != clients.end(); iter++)
	{
		std::string str((*iter).nickname);
		to_lower(str);
		if (str == aux)
			return false;
	}
	return true;
}

void Server::nick(const int& fd, Message& message)
{
	std::cout << YELLOW << "Command nick" << RESET << std::endl;
	std::vector<User>::iterator client = get_client_byfd(fd);
	if ((*client).is_online == false)
		return  send_message(fd, "CLIENT not connected, can't change nick");
	if (message.args.empty())
		return send_message(fd, ERR_NONICKNAMEGIVEN((*client).get_fullname()));
	if (message.args.size() > 1 
		|| message.args[0][0] == '$' || message.args[0][0] == ':'
		|| message.args[0][0] == '&' || message.args[0][0] == '#'
		|| message.args[0].find(" ,*?!@.") != std::string::npos)
		return send_message(fd, "ERROR nick erróneo");
	if (!check_availability(message.args[0], (*client).nickname))
		return send_message(fd, "ERROR nick ya en uso");
	(*client).nickname = message.args[0];
	//change nickname in all channels with necessary messages
	send_message(fd, "Enviar  mensaje correcto de haber cambiado el nick");
	//validate user
	
}


void Server::user(const int& fd, Message& message)
{
	(void)fd;
	(void)message;
	std::cout << LIGHT_BLUE << "Command user" << RESET << std::endl;
}


void Server::whois(const int& fd, Message& message)
{
	
(void)fd;
	(void)message;




}