#include "ft_irc.hpp"

// void Server::pass(const int& fd, Message& message)
// {
// 	std::cout << ORANGE << "Command password" << RESET << std::endl;
// 	(void)fd;
// 	(void)message;
	

// 	Client *client = *(get_client_byfd(fd));
// 	std::cout << LIGHT_GREEN << message.message << "-" << static_cast<int>(client->is_online) << "\n\t" << server_passwd << RESET << std::endl;
// 	if (client->is_online == true)
// 		return  send_message(fd, ERR_ALREADYREGISTERED(client->nickname));
// 	if (message.args.empty())
// 		return send_message(fd, ERR_NEEDMOREPARAMS(client->nickname));
// 	if (message.args[0] != server_passwd)
// 		return send_message(fd, ERR_PASSWDMISMATCH(client->nickname));
// 	client->is_online = true;
// 	std::cout << "[Server]: Client " << fd
// 		<< " from address " << client->hostname 
// 		<< " now has access to this IRC Server" << std::endl;

// }

void Server::pass(const int& fd, Message& message)
{
	Client*  client = *(get_client_byfd(fd));

	if (check_valid_user(client, message))
		return send_message(fd, ERR_ALREADYREGISTERED(client->nickname));
	if (message.args.empty())
		return send_message(fd, ERR_NEEDMOREPARAMS(client->nickname, "PASS"));
	if (message.args[0] != server_passwd)
	{
		send_message(fd, ERR_PASSWDMISMATCH(client->nickname));
		client->pass_tries++;
		if (client->pass_tries > 2)
		{
			send_message(fd, "[SERVER]: ERR_TOOMANYPASSTRIES\r\n");
			std::vector<struct pollfd>::iterator iter;
			for (iter = fds_poll.begin(); iter != fds_poll.end(); iter++)
			{
				if (iter->fd == fd)
					break ;
			}
			if (iter != fds_poll.end())
				remove_client((iter));
		}
		return ;
	}
	client->is_online = true;
	client->password = message.args[0];
	std::cout << "[Server]: Client " << fd
		<< " from address " << client->hostname 
		<< " now has access to this IRC Server" << std::endl;

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
		return  send_message(fd, "CLIENT not connected to server, can't change nick\r\n");
	if (message.args.empty())
		return send_message(fd, ERR_NONICKNAMEGIVEN((*client)->get_realname()));
	if (message.args.size() > 1 
		|| message.args[0][0] == '$' || message.args[0][0] == ':'
		|| message.args[0][0] == '&' || message.args[0][0] == '#'
		|| message.args[0].find(" ,*?!@.") != std::string::npos)
		return send_message(fd, "ERROR nick erróneo\r\n");
	if (!check_availability(message.args[0], (*client)->nickname))
		return send_message(fd, "ERROR nick ya en uso\r\n");

	std::string prev = (*client)->nickname;
	(*client)->nickname = message.args[0];

	//change nickname in all channels with necessary messages
	std::string msg  = ":" + prev  + " NICK " + (*client)->nickname + "\r\n"; 
	send_message(fd, RPL_CHANGENICK(prev, (*client)->nickname));
	std::cout  << ORANGE << "\t comprobar en nick" << RESET << std::endl;
	check_valid_user(*client, message);
}

void Server::user(const int& fd, Message& message)
{
	Client *client = *(get_client_byfd(fd));
	if (client->is_online == false)
		return  send_message(fd, "CLIENT not connected to server, can't change user\r\n");
	if (message.args.size() < 4)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "USER"));
	if (client->username.empty() == false)
		return send_message(fd, ERR_ALREADYREGISTERED(client->get_realname()));
	client->username = message.args[0];
	client->hostname = inet_ntoa(connection_addr.sin_addr);
	if (message.args.size() >= 4)
		client->realname = join_split(message.args, 3);
	if (client->realname[0] == ':')
		client->realname.erase(0, 1);
	std::cout  << ORANGE << "\t comprobar en user" << RESET << std::endl;
	std::cout << "-" << client->username << "-" << client->realname << "-" << client->hostname << "-" << std::endl;
	check_valid_user(client, message);
}

void Server::lusers(const int& fd, Message& message)
{
	(void)fd;
	(void)message;
	Client *client = *(get_client_byfd(fd));
	send_message(fd, RPL_LUSERCLIENT(client->nickname, int_to_string(client->server->clients.size())));
	send_message(fd, RPL_LUSEROP(client->nickname, int_to_string(5)));	//arreglar numero cuando tenga operadores
	// send_message(RPL_LUSERUNKNOWN(client->nickname));
	// revisar resto de repplies de LUSERS 
}

void Server::send_welcome(Client *client, Message &message)
{
	// (void)  client;
	std::cout << PINK << "enviar mensajes de  ahberse  podido conectar. solo pasa una vez" << std::endl;
	// std::cout << "[001] " << std::endl;
	// std::cout << "[002]" << std::endl;
	// std::cout << "[003]" << std::endl;
	// std::cout << "[004]" << RESET << std::endl;

	send_message(client->fd, RPL_WELCOME(client->nickname, client->get_realname()));
	send_message(client->fd, RPL_YOURHOST(client->nickname, client->hostname, "1.0"));
	send_message(client->fd, RPL_CREATED(client->nickname, get_time()));
	send_message(client->fd, RPL_MYINFO(client->nickname));
	send_message(client->fd, RPL_ISUPPORT(client->nickname));
	Server::lusers(client->fd, message);
	Server::motd(client->fd, message);
	//no  son prints, con enviar mensajes, pero lo que sea
}

void Server::motd(const int& fd, Message& message)
{
	(void)fd;
	(void)message;
	Client *client = *(get_client_byfd(fd));
	send_message(fd, RPL_MOTDSTART(client->nickname));
	send_message(fd, RPL_MOTD);
	send_message(fd, RPL_ENDOFMOTD(client->nickname));
}

bool Server::check_valid_user(Client *client, Message& message)
{
	std::cout << client->password.empty() <<"-"<< client->nickname.empty() <<"-"<< client->username.empty() << std::endl;
	if (client->password.empty() || client->nickname.empty() || client->username.empty() || (client->mode & VALID_CLIENT))
	{std::cout << "PRUEBA1" <<  std::endl;
		return false;
}
	if (!client->password.empty() && !client->nickname.empty() && !client->username.empty() && !(client->mode & VALID_CLIENT))
	{
		std::cout << "PRUEBA2" << std::endl;
		client->mode |= VALID_CLIENT;
		client->ping_request = false;
		send_welcome(client, message);
	}
	std::cout << "PRUEBA 3" << std::endl;
	return true;
}

void Server::whois(const int& fd, Message& message)
{
	(void)fd;
	(void)message;
	std::cout << "Command WOHIS" << std::endl;
}

void Server::quit(const int& fd, Message& message)
{

	//no está entrando aqui para 
	std::string msg = join_split(message.args, 0);
	msg = "QUIT: " + msg;
	// std::cout << YELLOW << msg << RESET << std::endl;
	send_message(fd, msg);

	std::vector<pollfd>::iterator iter;
	for (iter = fds_poll.begin(); iter != fds_poll.end(); iter++)
	{
		if (iter->fd == fd)
			break ;
	}
	if (iter  != fds_poll.end())
		remove_client(iter);
	//abandonar channel y notificar a usuarios en el canal


}

void Server::ping(const int& fd, Message& message)
{
	// Client *client = *(get_client_byfd(fd));
	if (message.args.size() < 1)
		return send_message(fd, "ERR_NEEDMOREPARAMS cosas cosas\r\n");
	std::string arg = join_split(message.args, 0);
	send_message(fd, "PONG: " + arg);
}

void Server::pong(const int& fd, Message& message)
{
	Client *client = *(get_client_byfd(fd));
	if (message.args.size() < 1)
		return send_message(fd, "mensaje  de que  tiene que aber un token. ERR_NEEDMOREPARAMS\r\n");
	if (client->ping_request == false)
		return ;
	if (message.args[0] == client->ping_token || message.args[0] == hostname)
	{
		client->ping_request = false;
		client->time_now = std::time(NULL);
	}
	else
	{
		send_message(fd, "ERROR TOKEN  INVALIDO\r\n");
		return ; //?¿?¿?¿?¿?¿?¿?¿? investigar  como salir de aqui;
	}
}

/*
*	[JOIN]
*		- Si el canal no existe, se crea.
*		- Si el canal existe, se une.
*		- Si el canal está bloqueado, se notifica.
*/
//TODO: Añadir unirse a más de un canal
void	Server::join(const int& fd, Message& message) //TODO
{
	Client *client = *(get_client_byfd(fd));
	std::string channel_name = message.args[0];

	if (channel_name[0] != '#')
		channel_name = "#" + channel_name;

	if (is_valid_channel_name(channel_name) == false)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "JOIN"));

	if (find_channel(channel_name))
		join_channel(channel_name, client, message);
	else
		create_channel(channel_name, client);
}

/*
*	[MODE]
*		- Si el cliente no es operador, se notifica.
*		- Si el modo no es válido, se notifica.
*		- Si el modo es válido, se cambia y se notifica a los usuarios del canal.
*/
void	Server::mode(const int& fd, Message& message)
{
	Client *client = *(get_client_byfd(fd));
	Channel *channel;
	std::vector<std::string> args = message.args;
	std::string modes_change;

	if (args.size() < 1)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "MODE"));
	if (find_channel(args[0]) == false)
		return send_message(fd, ERR_NOSUCHCHANNEL(client->get_realname(), args[0]));

	channel = *(get_channel_by_name(args[0]));
	if (channel->is_operator(client) == false)
		return send_message(fd, ERR_CHANOPRIVSNEEDED(client->get_realname(), args[0]));
	if (is_valid_mode(args[1], client) == false)
		return;
	modes_change = channel->handle_mode(args);
	if (modes_change.length() > 2)
		channel->broadcast_message(RPL_MODE(channel->get_name(), client->nickname, modes_change));
}

/*
*	[PRIVMSG]
*		- Si el canal no existe, se notifica.
*		- Si el canal existe, se envía el mensaje.
*/
//TODO: REPLIES
void	Server::privmsg(const int& fd, Message& message)
{
	Client *client = *(get_client_byfd(fd));
	Channel *channel = *(get_channel_by_name(message.args[0]));
	if (channel == NULL)
		return ;
	std::string msg = join_split(message.args, 1);
	if (msg.empty())
		return;
	if (msg[0] == ':')
		msg.erase(0, 1);
	channel->send_message(client, RPL_PRIVMSG(client->nickname, channel->get_name(), msg));
}

/*
*	[PART]
*		- Si faltan argumentos, se notifica.
*		- Si el canal no existe, se notifica.
*		- Si el cliente no está en el canal, se notifica.
*		- Si el cliente está en el canal, se elimina.
*/

void	Server::part(const int& fd, Message& message)
{
	Client *client = *(get_client_byfd(fd));
	if (message.args.size() < 1)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "PART"));

	Channel *channel = *(get_channel_by_name(message.args[0]));
	if (channel == NULL)
		return send_message(fd, ERR_NOSUCHCHANNEL(client->get_realname(), message.args[0]));

	if (client->is_in_channel(channel->get_name()) == false)
		return send_message(fd, ERR_NOTONCHANNEL(client->get_realname(), message.args[0]));
	client->leave_channel(channel);
	channel->broadcast_message(RPL_PART(client->get_realname(), channel->get_name()));
	channel->remove_client(client);
	std::cout << "Client " << client->get_realname() << " has left channel " << channel->get_name() << std::endl;
}

/*
*	[TOPIC]
*		- Si falta el argumento, se notifica.
*		- Si el canal no existe, se notifica.
*		- Si el cliente no está en el canal, se notifica.
*		- Si el cliente está en el canal, pero no es operador, se notifica.
*		- Si no hay topic, se notifica.
*/
void	Server::topic(const int& fd, Message& message)
{
	Client *client = *(get_client_byfd(fd));

	if (message.args.size() < 1)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "TOPIC"));

	Channel *channel = *(get_channel_by_name(message.args[0]));
	if (channel == NULL)
		return send_message(fd, ERR_NOSUCHCHANNEL(client->get_realname(), message.args[0]));

	if (message.args.size() < 2 || message.args[1] == ":")
		return send_message(fd, RPL_TOPIC(client->get_realname(), channel->get_name(), channel->get_topic()));

	std::string topic = join_split(message.args, 1);
	if (topic.length() > 0 && topic[0] == ':')
		topic.erase(0, 1);
	if (client->is_in_channel(channel->get_name()) == false)
		return send_message(fd, ERR_NOTONCHANNEL(client->get_realname(), channel->get_name()));

	if (channel->get_mode().t == false || channel->is_operator(client))
	{
		channel->set_topic(topic);
		client->send_message(RPL_TOPIC(client->get_realname(), channel->get_name(), channel->get_topic()));
		channel->broadcast_message(RPL_NOTICETOPIC(client->get_realname(), channel->get_name(), topic));
		return;
	}
	else
		return send_message(fd, ERR_CHANOPRIVSNEEDED(client->get_realname(), channel->get_name()));
}