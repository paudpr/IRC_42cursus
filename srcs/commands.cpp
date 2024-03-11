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

	if (check_valid_user(client, message) || !client->password.empty())
		return send_message(fd, ERR_ALREADYREGISTERED(message.cmd, client->nickname));
	if (message.args.empty())
		return send_message(fd, ERR_NEEDMOREPARAMS(client->nickname, "PASS"));
	if (message.args[0] != server_passwd)
	{
		// std::cout << PINK << "cosas cosas cosas cosas " << RESET << std::endl;
		send_message(fd, ERR_PASSWDMISMATCH(message.cmd));
		client->pass_tries++;
		if (client->pass_tries > 2)
		{
			send_message(fd, RPL_NONE(message.cmd, "Too many incorrect pass tries"));
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
	client->pass_tries = 0;
	std::cout << "[Server]: Client " << fd
		<< " from address " << client->hostname 
		<< " now has access to this IRC Server" << std::endl;
	send_message(fd, RPL_NONE(message.cmd, "You now have access to this IRC Server"));
	check_valid_user(client, message);
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
	if ((*client)->is_online == false && !(*client)->password.empty())
		return  send_message(fd, RPL_NONE(message.cmd, "Not connected to server, can't change nick. Please insert server password"));
	if (message.args.empty())
		return send_message(fd, ERR_NONICKNAMEGIVEN((*client)->get_realname()));
	if (message.args.size() > 1 
		|| message.args[0][0] == '$' || message.args[0][0] == ':'
		|| message.args[0][0] == '&' || message.args[0][0] == '#'
		|| message.args[0].find_first_of(" ,*?!@._") != std::string::npos)
		return send_message(fd, ERR_ERRONEUSNICKNAME(message.cmd));
	if (!check_availability(message.args[0], (*client)->nickname))
		return send_message(fd, ERR_NICKNAMEINUSE(message.cmd));

	std::string prev = (*client)->nickname;
	(*client)->nickname = message.args[0];

	//change nickname in all channels with necessary messages

	std::string msg  = ":" + prev  + " NICK " + (*client)->nickname + IRC_ENDLINE; 
	send_message(fd, RPL_CHANGENICK(prev, (*client)->nickname));
	check_valid_user(*client, message);
}

void Server::user(const int& fd, Message& message)
{
	Client *client = *(get_client_byfd(fd));
	if (check_valid_user(client, message))
		return send_message(fd, ERR_ALREADYREGISTERED(message.cmd, client->nickname));
	if (client->is_online == false && !client->password.empty())
		return  send_message(fd, RPL_NONE(message.cmd, "Not connected to server, can't change user. Please insert server password"));
	if (message.args.size() < 4)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "USER"));
	if (client->username.empty() == false)
		return send_message(fd, ERR_ALREADYREGISTERED(message.cmd, client->get_realname()));
	client->username = message.args[0];
	client->hostname = inet_ntoa(connection_addr.sin_addr);
	if (message.args.size() >= 4)
		client->realname = join_split(message.args, 3);
	if (client->realname[0] == ':')
		client->realname.erase(0, 1);
	check_valid_user(client, message);
}

void Server::lusers(const int& fd, Message& message)
{
	(void)message;
	Client *client = *(get_client_byfd(fd));
	int num_opers = client->server->clients.size();
	send_message(fd, RPL_LUSERCLIENT(client->nickname, int_to_string(client->server->clients.size())));
	send_message(fd, RPL_LUSEROP(client->nickname, "0"));	//arreglar numero cuando tenga operadores
	send_message(fd, RPL_LUSERUNKNOWN(client->nickname));
	send_message(fd, RPL_LUSERCHANNELS(client->nickname, int_to_string(client->server->channels.size())));
	send_message(fd, RPL_LUSERME(client->nickname, int_to_string(client->server->clients.size()), "1"));
	send_message(fd, RPL_lOCALUSERS(client->nickname, int_to_string(client->server->clients.size()), "1")); //Tercer argumento es el maximo de clientes conectados
	send_message(fd, RPL_GLOBALUSERS(client->nickname, "0", "0")); //Tercer argumento es el maximo de clientes conectados
}

void Server::send_welcome(Client *client, Message &message)
{
	// (void)  client;
	std::cout << PINK << "enviar mensajes de  ahberse  podido conectar. solo pasa una vez" << std::endl;
	// std::cout << "[001] " << std::endl;
	// std::cout << "[002]" << std::endl;
	// std::cout << "[003]" << std::endl;
	// std::cout << "[004]" << RESET << std::endl;

	send_message(client->fd, RPL_WELCOME(client->nickname, client->username, client->hostname));
	send_message(client->fd, RPL_YOURHOST(client->nickname, client->hostname, "1.0"));
	send_message(client->fd, RPL_CREATED(client->nickname, get_time()));
	send_message(client->fd, RPL_MYINFO(client->username, "ft_irc", "1.0", "-", "itklo"));
	send_message(client->fd, RPL_ISUPPORT(client->nickname));
	Server::lusers(client->fd, message);
	Server::motd(client->fd, message);
}

void Server::motd(const int& fd, Message& message)
{
	(void)message;
	Client *client = *(get_client_byfd(fd));
	send_message(fd, RPL_MOTDSTART(client->nickname));
	send_message(fd, RPL_MOTD(client->nickname));
	send_message(fd, RPL_ENDOFMOTD(client->nickname));
}

bool Server::check_valid_user(Client *client, Message& message)
{
	std::cout << client->password.empty() <<"-"<< client->nickname.empty() <<"-"<< client->username.empty() << std::endl;
	if (client->password.empty() || client->nickname.empty() || client->username.empty() || (client->mode & VALID_CLIENT))
		return false;
	if (!client->password.empty() && !client->nickname.empty() && !client->username.empty() && !(client->mode & VALID_CLIENT))
	{
		client->mode |= VALID_CLIENT;
		client->ping_request = false;
		send_welcome(client, message);
	}
	return true;
}

//TODO: Replies
void Server::whois(const int& fd, Message& message)
{
	Client *client = *(get_client_byfd(fd));
	if (message.args.size() < 1)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "WHOIS"));
	std::string nick = message.args[0];
	Client *whois = get_client_by_nickname(nick);
	if (whois == NULL)
		return send_message(fd, ERR_NOSUCHNICK(client->get_realname(), nick));
	
	std::cout << "Command WOHIS" << std::endl;
}

void Server::quit(const int& fd, Message& message)
{
	std::vector<pollfd>::iterator iter;
	for (iter = fds_poll.begin(); iter != fds_poll.end(); iter++)
	{
		if (iter->fd == fd)
			break ;
	}
	if (iter != fds_poll.end())
		remove_client(iter);
	std::string msg = join_split(message.args, 0);
	send_message(fd, RPL_QUIT(message.cmd, msg));
}

void Server::ping(const int& fd, Message& message)
{
	Client *client = *(get_client_byfd(fd));
	if (message.args.size() < 1)
		return send_message(fd, ERR_NEEDMOREPARAMS(message.cmd, client->nickname));
	std::string arg = join_split(message.args, 0);
	send_message(fd, "PONG: " + arg);
}

void Server::pong(const int& fd, Message& message)
{
	Client *client = *(get_client_byfd(fd));
	if (message.args.size() < 1)
		return send_message(fd, ERR_NEEDMOREPARAMS(message.cmd, client->nickname));
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
	Channel *channel = *(get_channel_by_name(channel_name));
	channel->increase_clients();
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
*	Envía un mensaje a un canal o a un usuario.
*		- Si el canal no existe, se notifica.
*		- Si el canal existe, se envía el mensaje.
*/
//TODO: ERR_NOSUCHNICK (401)
//TODO: ERR_NOSUCHSERVER (402)
//TODO: ERR_CANNOTSENDTOCHAN (404)
//TODO: ERR_TOOMANYTARGETS (407)
//TODO: ERR_NORECIPIENT (411)
//TODO: ERR_NOTEXTTOSEND (412)
//TODO: ERR_NOTOPLEVEL (413)
//TODO: ERR_WILDTOPLEVEL (414)
//TODO: RPL_AWAY (301)
//TODO: comprobar que el cliente esté en el canal
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
*	Abandona el canal, si no queda nadie, se elimina.
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
	channel->decrease_clients();
	if (channel->get_current_clients() == 0)
		remove_channel(channel);
	std::cout << "Client " << client->get_realname() << " has left channel " << channel->get_name() << std::endl;
}

/*
*	[TOPIC]
*	Añade o modifica el topic del canal.
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

/*
*	[INVITE]
*	Invitar a un usuario a un canal.
*		- Si falta el argumento, se notifica.
*		- Si el canal no existe, se notifica.
*		- Si el cliente no está en el canal, se notifica.
*		- Si el cliente está en el canal, pero no es operador, y está bloqueado (+i), se notifica.
*		- Si el usuario al que se quiere invitar ya está en el canal, se notifica.
*/
void	Server::invite(const int& fd, Message& message)
{
	std::string	channel_name;
	std::string	nick;
	Client *client = *(get_client_byfd(fd));
	if (message.args.size() < 2)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "INVITE"));

	nick = message.args[0];
	channel_name = message.args[1];

	if (client_exists(nick) == false)
		return send_message(fd, ERR_NOSUCHNICK(client->get_realname(), nick));
	
	Channel *channel = *(get_channel_by_name(channel_name));
	if (channel == NULL)
		return send_message(fd, ERR_NOSUCHCHANNEL(client->get_realname(), channel_name));

	if (client->is_in_channel(channel_name) == false)
		return send_message(fd, ERR_NOTONCHANNEL(client->get_realname(), channel_name));

	if (channel->get_mode().i == true && channel->is_operator(client) == false)
		return send_message(fd, ERR_CHANOPRIVSNEEDED(client->get_realname(), channel_name));

	Client *invited = channel->find_client_by_nick(nick);
	if (invited != NULL)
		return send_message(fd, ERR_USERONCHANNEL(client->get_realname(), nick, channel_name));

	invited = get_client_by_nickname(nick);
	invited->invited_to(channel_name);
	invited->send_message(RPL_INVITING(client->get_realname(), invited->get_realname(), channel_name));
}

/*
*	[KICK]
*	Echar a un usuario del canal.
*		- Si falta el argumento, se notifica.
*		- Si el canal no existe, se notifica.
*		- Si no es operador, se notifica.
*		- Si el usuario no está en el canal, se notifica.
*		- Si el usa el comando no esta en el canal, se notifica.
*/
void	Server::kick(const int& fd, Message& message)
{
	(void)fd;
	(void)message;
	std::cout << "Command KICK" << std::endl;
}