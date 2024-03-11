#include "Server.hpp"

Server::Server() : online(true)
{
	save_commands();
}

Server::~Server() {
	for (std::vector<pollfd>::iterator iter = fds_poll.begin(); iter != fds_poll.end(); iter++)
		if (iter->fd != server_fd)
			close(iter->fd);
	close(server_fd);
}
Server::Server(const Server& copy)
{
	*this = copy;
}

Server&		Server::operator=(const Server& other)
{
	if (this != &other) {
		fds_poll = other.fds_poll;
		commands = other.commands;
		server_size = other.server_size;
		connection_size = other.connection_size;
		server_addr = other.server_addr;
		connection_addr = other.connection_addr;
		hostname = other.hostname;
		server_fd = other.server_fd;
		server_port = other.server_port;
		save_commands();
	}
	return *this;
}

std::string Server::get_hostname()
{
	char hostname[HOSTNAME_LEN];
	struct hostent* entry;
	std::string str;

	gethostname(hostname, HOSTNAME_LEN);
	entry = gethostbyname(hostname);
	if (!entry)
		str = "127.0.0.1";
	else 
		str = inet_ntoa(*((struct in_addr*)entry->h_addr_list[0]));
	return str;
}

void	Server::init()
{
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(server_port);
	server_size = sizeof(server_addr);
	connection_size = sizeof(connection_addr);
	hostname = get_hostname();

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		throw std::runtime_error("[ ERROR ] Socket failed");

	struct pollfd	server_poll_fd = (struct pollfd){server_fd, POLLIN, 0};
	fds_poll.push_back(server_poll_fd);

	int		aux = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &aux, sizeof(int)) < 0)
	{
		close(server_fd);
		throw std::runtime_error("[ ERROR ] Setsockopt address failed");
	}
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &aux, sizeof(int)) < 0)
	{
		close(server_fd);
		throw std::runtime_error("[ ERROR ] Setsockopt port failed");
	}
	if (bind(server_fd, (sockaddr*)&server_addr, server_size) < 0)
	{
		close(server_fd);
		throw std::runtime_error("[ ERROR ] Bind failed");
	}
}

void	Server::add_client(std::vector<pollfd>::iterator &iter) {
	int							fd_connection;
	struct pollfd				poll_connection;

	fd_connection = accept(iter->fd, (sockaddr*)&connection_addr, &connection_size);
	if (fd_connection < 0)
		throw std::runtime_error("[ ERROR ] Accept failed");	// cambiar a mensaje de error?
	if (clients.size() >= BACKLOG || fds_poll.size() >= BACKLOG + 1)
	{
		// send_msg( "Too many clients connected already"); // enviar mensaje al cliente de que no se puede conectar
		std::cout <<  "[ ERROR ] Too many client connections" << std::endl;
		close(fd_connection);
	}

	fcntl(fd_connection, F_SETFL, O_NONBLOCK);
	poll_connection = (struct pollfd){fd_connection, POLLIN, 0};
	fds_poll.push_back(poll_connection);
	iter = fds_poll.begin();

	Client	*client = new Client(fd_connection, connection_addr.sin_addr, this);
	client->is_online = true;
	client->ping_request = true;
	clients.push_back(client);
	
	std::cout << CYAN << "[Server]: Client " << fd_connection << " from " << inet_ntoa(connection_addr.sin_addr)
		<< ":" << ntohs(connection_addr.sin_port) << " connected." << RESET << std::endl;

	send_message(fd_connection, "Now connected to esteproyectoponlocomosea\r\n");
}

void	Server::remove_client(std::vector<pollfd>::iterator &iter)
{
	Client *client = *(get_client_byfd(iter->fd));
	client->is_online = false;

	//abandonar channels

	std::cout << RED << "[Server]: Client " << iter->fd
		<< " from " << inet_ntoa(connection_addr.sin_addr)
		<< ":" << ntohs(connection_addr.sin_port)
		<< " disconnected" << RESET << std::endl;

	int aux_fd = iter->fd;
	fds_poll.erase(iter);
	close(aux_fd);

	std::vector<Client*>::iterator it = std::find(clients.begin(), clients.end(), client);
	if (it != clients.end())
		clients.erase(it);
	else 
		std::cout << "There was a problem, client not found" << std::endl; 
	delete (client);
}

std::vector<Client*>::iterator Server::get_client_byfd(int fd)
{
	for (std::vector<Client*>::iterator iter = clients.begin(); iter != clients.end(); iter++)
	{
		if ((*iter)->fd == fd)
			return iter;
	}
	return clients.end();
}

void Server::do_communications(std::vector<pollfd>::iterator &iter)
{
	char buffer[BUFFER]; // [BUFFER + 1] ???????
	bzero(&buffer, sizeof(buffer));
	int read_bytes;

	std::vector<Client*>::iterator client = get_client_byfd(iter->fd);	//version sentido comun
	if ((*client)->mode & VALID_CLIENT)
	{
		(*client)->time_now = std::time(NULL);
		(*client)->ping_request = false;
	}

	read_bytes = recv(iter->fd, buffer, BUFFER, 0);
	std::cout << GREEN << buffer << std::endl;
	msg.clear();
	if (read_bytes <= 0)
	{
		// cambiar a ejecutar comando QUIT ?????
		remove_client(iter);
		return ;
	}

	msg = (*client)->receive_leftovers;
	msg.append(buffer);
	size_t pos = msg.find('\n');
	if (pos != std::string::npos)
		(*client)->receive_leftovers.clear();
	
	std::vector<std::string> split = split_msg();
	for (std::vector<std::string>::iterator it = split.begin(); it != split.end(); it++)
	{
		Message message(*it);
		std::vector<Server::ptr>::iterator cmd = get_command(message.cmd); 
		if (cmd != commands.end())
		{

			std::cout << PINK << message.message << RESET << std::endl;
			// std::cout  << static_cast<int>(check_valid_user(*client)) <<  std::endl;
			//!BYPASS
				// (this->*(*cmd))((*client)->fd, message);
			if (message.cmd == "PASS" || message.cmd == "QUIT" || message.cmd == "USER" || message.cmd == "NICK" 
				|| check_valid_user(*client, message))
				(this->*(*cmd))((*client)->fd, message);
			else
				send_message((*client)->fd, get_time() + (*client)->nickname + " :Not connected to server\r\n");
		}
	}
}

std::vector<std::string> Server::split_msg(void)
{
	std::stringstream stream(msg);
	std::string str;
	std::vector<std::string> split;

	while (std::getline(stream, str, '\n'))
	{
		size_t last_pos = str.find_last_of('\r');
		if (last_pos != std::string::npos)
			str.erase(last_pos);
		split.push_back(str);
	}
	return (split);
}

void	Server::start()
{
	if (listen(server_fd, BACKLOG) < 0)
	{
		close(server_fd);
		throw std::runtime_error("[ ERROR ] Listen failed");
	}

	time_t		time_now;
	time(&time_now);
	time_init = ctime(&time_now);
	time_init.resize(time_init.size() - 1);

	std::cout << GREEN << "IRC Server started and listening at " << server_port << RESET << std::endl;

	//gestionar  tiempo

	while (online) {
		int poll_count = poll(fds_poll.begin().base(), (nfds_t)fds_poll.size(), POLL_TIMEOUT_MS);
		if (poll_count < 0)
			throw std::runtime_error("[ ERROR ] Poll");
		for (std::vector<pollfd>::iterator iter = fds_poll.begin(); iter < fds_poll.end(); iter++)
		{
			if (iter->revents & POLLIN)
			{
				if (iter->fd == server_fd)
					add_client(iter);
				else 
					do_communications(iter);
			}
			else if (iter->revents & POLLOUT) //filedescritor is ready for writing
			{
				if (!(*(get_client_byfd(iter->fd)))->send_leftovers.empty())
				{
					std::cout << "gestionar restos" << std::endl;
					// send_message(iter->fd, (*(get_client_byfd(iter->fd))).send_leftovers);
					(*(get_client_byfd(iter->fd)))->send_leftovers.clear();
				}
			}
			else if (iter->revents & POLLHUP)
				remove_client(iter);
		}
		// check_ping();
	}
	std::cout << BLUE << "Ending server. Bye!" << std::endl;
}

// not necessary for colloquy client
// void Server::check_ping()
// {
// 	for (std::vector<pollfd>::iterator iter = fds_poll.begin(); iter != fds_poll.end(); iter++)
// 	{
// 		if (iter->fd != server_fd)
// 		{
// 			Client  *client = *(get_client_byfd(iter->fd));
// 			if (client->ping_request && std::time(NULL) - client->time_init > PING_TIMEOUT)
// 			{
// 				send_message(client->fd, "[ ERROR ] Timeout");
// 				remove_client(iter);
// 			}
// 			else if (client->ping_request ==  false && std::time(NULL) - client->time_now > PING_FREQ)
// 			{
// 				std::cout << "está entrando aqui" << std::endl;
// 				client->ping_request = true;
// 				client->ping_token = generate_token();
// 				client->time_now = std::time(NULL);
// 				send_message(client->fd, "PING: " + client->ping_token);
// 			}
// 		}
// 	}
// }

std::vector<Server::ptr>::iterator Server::get_command(std::string& name)
{
	if (name == "PASS") return std::find(commands.begin(), commands.end(), &Server::pass);
	if (name == "NICK") return std::find(commands.begin(), commands.end(), &Server::nick);
	if (name == "USER") return std::find(commands.begin(), commands.end(), &Server::user);
	if (name == "WHOIS") return std::find(commands.begin(), commands.end(), &Server::whois);
	if (name == "QUIT")  return  std::find(commands.begin(), commands.end(), &Server::quit);
	if (name == "PING") return std::find(commands.begin(), commands.end(), &Server::ping);
	if (name == "PONG") return std::find(commands.begin(), commands.end(), &Server::pong);
	if (name == "LUSERS") return std::find(commands.begin(), commands.end(), &Server::lusers);
	if (name == "MOTD") return std::find(commands.begin(), commands.end(),  &Server::motd);
	if (name == "JOIN") return std::find(commands.begin(), commands.end(),  &Server::join);
	if (name == "MODE") return std::find(commands.begin(), commands.end(),  &Server::mode);
	if (name == "PRIVMSG") return std::find(commands.begin(), commands.end(),  &Server::privmsg);
	if (name == "PART") return std::find(commands.begin(), commands.end(),  &Server::part);
	if (name == "TOPIC") return std::find(commands.begin(), commands.end(),  &Server::topic);
	if (name == "INVITE") return std::find(commands.begin(), commands.end(),  &Server::invite);
	if (name == "KICK") return std::find(commands.begin(), commands.end(),  &Server::kick);
	return (commands.end());
}

void Server::save_commands()
{
	commands.push_back(&Server::pass);
	commands.push_back(&Server::nick);
	commands.push_back(&Server::user);
	commands.push_back(&Server::quit);
	commands.push_back(&Server::ping);
	commands.push_back(&Server::pong);
	commands.push_back(&Server::lusers);
	commands.push_back(&Server::motd);
	commands.push_back(&Server::join);
	commands.push_back(&Server::mode);
	commands.push_back(&Server::privmsg);
	commands.push_back(&Server::part);
	commands.push_back(&Server::topic);
	commands.push_back(&Server::invite);
	commands.push_back(&Server::kick);
}

void Server::send_message(const int &fd, std::string message)
{
	size_t read = send(fd, message.c_str(), message.size(), 0);
	if (read < 0)
	{
		std::cout << "[ ERROR ] Send() failed" <<  std::endl;
		return ;
	}
	if (read < message.size())
	{
		std::vector<Client*>::iterator client = get_client_byfd(fd);
		(*client)->send_leftovers = message.substr(read);
	}
	std::cout << GREY << get_time() << ": Sent to client [fd=" << fd << "] message:\n\t" << message << RESET << std::endl;
}

bool	Server::find_channel(std::string name) //TODO
{
	std::vector<Channel *>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it)
		if ((*it)->get_name() == name)
			return (true);
	return (false);
}

std::vector<Channel*>::iterator Server::get_channel_by_name(std::string name)
{
	for (std::vector<Channel*>::iterator iter = channels.begin(); iter != channels.end(); iter++)
	{
		if ((*iter)->get_name() == name)
			return iter;
	}
	return channels.end();
}

bool	Server::is_valid_channel_name(std::string name)
{
	if (name[0] != '#' || name.size() < 2)
		return false;
	for (size_t i = 1; i < name.size(); i++)
	{
		if (name[i] == ' ' || name[i] == ',' || name[i] == ':' || name[i] == '\r' || name[i] == '\n')
			return false;
	}
	return true;
}

void	Server::create_channel(std::string name, Client *client)
{
	Channel *channel = new Channel(name, client);
	this->add_channel(channel);
	client->join_channel(channel);
	send_message(client->fd, RPL_JOIN(client->get_realname(), name));
	channel->broadcast_message(RPL_MODE(channel->get_name(), std::string("ft_irc"), "+to " + client->nickname));
}

void	Server::join_channel(std::string name, Client *client, Message& message) //TODO
{
	Channel *channel = *(get_channel_by_name(name));

	if (can_join_channel(client, channel, message.args) == false)
		return ;
	client->join_channel(channel);
	channel->add_client(client);
	std::cout << RPL_JOIN(client->get_realname(), name) << std::endl;
	channel->broadcast_message(RPL_JOIN(client->get_realname(), name));
	if (channel->get_topic().empty())
		send_message(client->fd, RPL_TOPIC(client->get_realname(), channel->get_name(), channel->get_topic()));
	send_message(client->fd, RPL_NAMREPLY(client->get_realname(), channel->get_name(), channel->get_list_of_clients()));
	send_message(client->fd, RPL_ENDOFNAMES(client->get_realname(), channel->get_name()));
}

bool	Server::can_join_channel(Client *client, Channel *channel, std::vector<std::string> &args) //TODO
{
	if (channel->get_mode().k == true)
	{
		if (args[1] != channel->get_password())
			send_message(client->fd, ERR_BADCHANNELKEY(client->get_realname(), channel->get_name()));
		else
			return (true);
	}
	else if (channel->get_mode().i == true)
	{
		if (client->is_invited_to(channel->get_name()) == false)
			send_message(client->fd, ERR_INVITEONLYCHAN(client->get_realname(), channel->get_name()));
		else
		{
			client->remove_invitation(channel->get_name());
			return (true);
		}
	}
	else if(channel->get_mode().l == true)
	{
		if (channel->get_max_clients() <= channel->get_current_clients())
			send_message(client->fd, ERR_CHANNELISFULL(client->get_realname(), channel->get_name()));
		else
			return (true);
	}
	else
		return (true);
	return (false);
}

void	Server::add_channel(Channel *channel)
{
	channels.push_back(channel);
}

bool	Server::is_valid_mode(std::string mode, Client *client)
{
	std::string flags_op;

	if (mode[0] != '+' && mode[0] != '-')
		return false;
	flags_op = mode.substr(1);
	for(size_t i = 0; i < flags_op.size(); i++)
		if (flags_op[i] != 'i' && flags_op[i] != 't' && flags_op[i] != 'k' && flags_op[i] != 'o' && flags_op[i] != 'l')
		{
			send_message(client->fd, ERR_UNKNOWNMODE(client->get_realname(), flags_op[i]));
			return false;
		}
	return true;
}

bool	Server::client_exists(std::string nickname)
{
	for (std::vector<Client*>::iterator iter = clients.begin(); iter != clients.end(); iter++)
	{
		if ((*iter)->nickname == nickname)
			return true;
	}
	return false;
}

Client *Server::get_client_by_nickname(std::string nickname)
{
	for (std::vector<Client*>::iterator iter = clients.begin(); iter != clients.end(); iter++)
	{
		if ((*iter)->nickname == nickname)
			return *iter;
	}
	return NULL;
}

void	Server::remove_channel(Channel *channel)
{
	std::vector<Channel*>::iterator it = std::find(channels.begin(), channels.end(), channel);
	if (it != channels.end())
		channels.erase(it);
	else 
		std::cout << "There was a problem, channel not found" << std::endl; 
	delete (channel);
}