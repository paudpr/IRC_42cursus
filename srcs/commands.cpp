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
	// std::cout  << ORANGE << "\t comprobar en nick" << RESET << std::endl;
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
	// std::cout  << ORANGE << "\t comprobar en user" << RESET << std::endl;
	// std::cout << "-" << client->username << "-" << client->realname << "-" << client->hostname << "-" << std::endl;
	check_valid_user(client, message);
}

void Server::lusers(const int& fd, Message& message)
{
	(void)fd;
	(void)message;
	Client *client = *(get_client_byfd(fd));
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
	// std::cout << PINK << "enviar mensajes de  ahberse  podido conectar. solo pasa una vez" << std::endl;
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
	if (client->password.empty() || client->nickname.empty() || client->username.empty() || (client->mode & VALID_CLIENT))
		return false;
	if (!client->password.empty() && !client->nickname.empty() && !client->username.empty() && !(client->mode & VALID_CLIENT))
	{
		// std::cout << "PRUEBA2" << std::endl;
		client->mode |= VALID_CLIENT;
		client->ping_request = false;
		send_welcome(client, message);
	}
	// std::cout << "PRUEBA 3" << std::endl;
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
	Client *client = *(get_client_byfd(fd));
	if (message.args.size() < 1)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "PING"));
	std::string arg = join_split(message.args, 0);
	send_message(fd, "PONG: " + arg);
}

void Server::pong(const int& fd, Message& message)
{
	std::cout << ORANGE <<  "POOOOOOONNNNNGGGGGGG" << RESET << std::endl; 
	Client *client = *(get_client_byfd(fd));
	if (message.args.size() < 1)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "PONG"));
	if (client->ping_request == false)
		return ;
	if (message.args[0] == client->ping_token || message.args[0] == hostname)
	{
		client->ping_request = false;
		client->time_now = std::time(NULL);
	}
	else
		send_message(fd, RPL_NONE(client->get_realname(), "Token not valid"));
}

/*
*	[JOIN] "/JOIN <#channel>[,<channel>...] [password]"
*		- Si el canal no existe, se crea.
*		- Si el canal existe, se une.
*		- Si el canal está bloqueado, se notifica.
*/
//? Testear
void	Server::join(const int& fd, Message& message)
{
	Client *client = *(get_client_byfd(fd));
	std::string channel_name;
	std::string password;
	std::string args = join_split(message.args, 0);
	std::vector<std::string> channels = split(args, ',');
	std::vector<std::string>::iterator iter;
	std::vector<std::string> channel_args;
	for (iter = channels.begin(); iter != channels.end(); iter++)
	{
		channel_args = split(*iter, ' ');
		channel_name = channel_args[0];
		if (channel_name.empty())
			return ;
		if (channel_args.size() > 1)
			password = channel_args[1];
		else
			password = "";
		if (channel_name[0] != '#')
			channel_name = "#" + channel_name;
		if (is_valid_channel_name(channel_name) == false)
			return send_message(fd, ERR_NOSUCHCHANNEL(client->get_realname(), channel_name));
		if (find_channel(channel_name) == false)
			create_channel(channel_name, client);
		else
			join_channel(client, channel_name, password);
	}
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
	std::vector<Channel *>::iterator channel_it;
	std::vector<std::string> args = message.args;
	std::string modes_change;

	if (args.size() < 1)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "MODE"));
	
	channel_it =  get_channel_by_name(args[0]);
	if (channel_it == channels.end())
		return send_message(fd, ERR_NOSUCHCHANNEL(client->get_realname(), args[0]));
	channel = *channel_it;

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
//TODO: ERR_NOTOPLEVEL (413)
//TODO: ERR_WILDTOPLEVEL (414)
// //TODO: RPL_AWAY (301)
//TODO: comprobar que el cliente esté en el canal
void	Server::privmsg(const int& fd, Message& message)
{
	std::vector<Client *>::iterator client_it = get_client_byfd(fd);
	if (client_it == clients.end())
		return ;
	Client *client = *client_it;
	if (message.args.size() == 0)
		return send_message(fd, ERR_NORECIPIENT(client->get_realname(), "PRIVMSG"));
	if (message.args.size() == 1)
		return send_message(fd, ERR_NOTEXTTOSEND(client->get_realname()));
	std::string	target = message.args[0];
	std::vector<Channel *>::iterator channel_it = get_channel_by_name(target);
	if (channel_it != channels.end())
	{
		Channel *channel = *channel_it;
		if (client->is_in_channel(channel->get_name()) == false)
			return ;
		std::string msg = join_split(message.args, 1);
		if (msg.empty())
			return;
		if (msg[0] == ':')
			msg.erase(0, 1);
		if (client->is_in_channel(channel->get_name()) == false)
			return send_message(fd, ERR_CANNOTSENDTOCHAN(client->get_realname(), channel->get_name()));
		channel->send_message(client, RPL_PRIVMSG(client->nickname, channel->get_name(), msg));
	}
	else if (client_exists(target))
	{
		Client *target_client = get_client_by_nickname(target);
		std::string msg = join_split(message.args, 1);
		if (msg.empty())
			return;
		if (msg[0] == ':')
			msg.erase(0, 1);
		target_client->send_message(RPL_PRIVMSG(client->nickname, target_client->nickname, msg));
	}
	else
		send_message(fd, ERR_NOSUCHNICK(client->get_realname(), target));
}

/*
*	[PART]
*	Abandona el canal, si no queda nadie, se elimina.
*		- Si faltan argumentos, se notifica.
*		- Si el canal no existe, se notifica.
*		- Si el cliente no está en el canal, se notifica.
*		- Si el cliente está en el canal, se elimina.
*/
//?Testear
void	Server::part(const int& fd, Message& message)
{
	Client *client = *(get_client_byfd(fd));
	Channel *channel;
	std::vector<Channel *>::iterator channel_it;
	std::string channel_name;
	std::string message_part;
	std::string args;
	std::vector<std::string> channels_names;
	std::vector<std::string> last_args;
	std::vector<std::string>::iterator iter;

	if (message.args.size() < 1)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "PART"));

	args = join_split(message.args, 0);
	channels_names = split(args, ',');
	last_args = split(channels_names.back(), ' ');
	if (last_args.size() > 1)
		message_part = join_split(last_args, 1);
	else
		message_part = "";
	channels_names.pop_back();
	channels_names.push_back(last_args[0]);
	for (iter = channels_names.begin(); iter != channels_names.end(); ++iter)
	{
		channel_name = *iter;
		if (channel_name.empty())
			return ;
		channel_it = get_channel_by_name(channel_name);
		if (channel_it == channels.end())
			return send_message(fd, ERR_NOSUCHCHANNEL(client->get_realname(), channel_name));
		channel = *channel_it;

		if (client->is_in_channel(channel->get_name()) == false)
			return send_message(fd, ERR_NOTONCHANNEL(client->get_realname(), channel_name));

		client->leave_channel(channel);
		channel->broadcast_message(RPL_PART(client->get_realname(), channel->get_name(), message_part));
		channel->remove_client(client);
		channel->decrease_clients();
		if (channel->get_current_clients() == 0)
			remove_channel(channel);
		std::cout << "Client " << client->get_realname() << " has left channel " << channel->get_name() << std::endl;
	}
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
//?Testear
void	Server::topic(const int& fd, Message& message)
{
	Channel *channel;
	std::vector<Channel *>::iterator channel_it;
	Client *client = *(get_client_byfd(fd));

	if (message.args.size() < 1)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "TOPIC"));

	channel_it = get_channel_by_name(message.args[0]);
	if (channel_it == channels.end())
		return send_message(fd, ERR_NOSUCHCHANNEL(client->get_realname(), message.args[0]));
	channel = *channel_it;

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
//?Testear
void	Server::invite(const int& fd, Message& message)
{
	std::string	channel_name;
	std::string	nick;
	Channel *channel;
	std::vector<Channel *>::iterator channel_it;
	Client *client = *(get_client_byfd(fd));
	if (message.args.size() < 2)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "INVITE"));

	nick = message.args[0];
	channel_name = message.args[1];

	if (client_exists(nick) == false)
		return send_message(fd, ERR_NOSUCHNICK(client->get_realname(), nick));
	
	
	channel_it = get_channel_by_name(channel_name);
	if (channel_it == channels.end())
		return send_message(fd, ERR_NOSUCHCHANNEL(client->get_realname(), channel_name));
	channel = *channel_it;

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
//TODO: Echar mas de un usuario, y añadir un mensaje.
void	Server::kick(const int& fd, Message& message)
{
	Channel *channel;
	std::vector<Channel *>::iterator channel_it;
	Client *client = *(get_client_byfd(fd));
	if (message.args.size() < 2)
		return send_message(fd, ERR_NEEDMOREPARAMS(client->get_realname(), "KICK"));

	std::string	channel_name = message.args[0];
	std::string	nick = message.args[1];

	channel_it = get_channel_by_name(channel_name);
	if (channel_it == channels.end())
		return send_message(fd, ERR_NOSUCHCHANNEL(client->get_realname(), channel_name));
	channel = *channel_it;

	if (channel->is_operator(client) == false)
		return send_message(fd, ERR_CHANOPRIVSNEEDED(client->get_realname(), channel_name));

	Client *kicked = channel->find_client_by_nick(nick);
	if (kicked == NULL)
		return send_message(fd, ERR_USERNOTINCHANNEL(client->get_realname(), nick, channel_name));

	if (client->is_in_channel(channel_name) == false)
		return send_message(fd, ERR_NOTONCHANNEL(client->get_realname(), channel_name));

	channel->broadcast_message(RPL_KICK(client->get_realname(), channel_name, nick));
	channel->remove_client(kicked);
	kicked->leave_channel(channel);
}

/*
*	[LIST]
*	Lista los canales y sus usuarios.
*/
//?Testear
void	Server::list(const int& fd, Message& message)
{
	std::string channel_join;
	std::vector<std::string> channel_list;
	std::vector<Channel *>::iterator iter;
	std::vector<Channel *> channels_to_list;
	std::vector<Client *>::iterator client_it = get_client_byfd(fd);
	if (client_it == clients.end())
		return ;
	Client *client = *client_it;
	send_message(fd, RPL_LISTSTART(client->get_realname()));
	if (message.args.size() > 0)
	{
		channel_join = join_split(message.args, 0);
		channel_list = split(channel_join, ',');
		for (std::vector<std::string>::iterator iter = channel_list.begin(); iter != channel_list.end(); iter++)
			if (find_channel(*iter))
				channels_to_list.push_back(*get_channel_by_name(*iter));
	}
	for (iter = channels_to_list.begin(); iter != channels_to_list.end(); iter++)
		send_message(fd, RPL_LIST(client->get_realname(), (*iter)->get_name(), int_to_string((*iter)->get_current_clients()), (*iter)->get_topic()));
	send_message(fd, RPL_LISTEND(client->get_realname()));
}