#include "Server.hpp"

Server::Server() : online(true)
{
	save_commands();
	save_opers();
}

Server::~Server() 
{
	for (std::vector<pollfd>::iterator iter = fds_poll.begin(); iter != fds_poll.end(); iter++)
		if (iter->fd != server_fd)
			close(iter->fd);
	close(server_fd);
	online = false;
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




// Function to convert decimal to hexadecimal string
std::string decToHex(unsigned long long decVal) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << decVal;
    return ss.str();
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


void	Server::add_client(std::vector<pollfd>::iterator &iter)
{
	int							fd_connection;
	struct pollfd				poll_connection;

	fd_connection = accept(iter->fd, (sockaddr*)&connection_addr, &connection_size);
	if (fd_connection < 0)
		throw std::runtime_error("[ ERROR ] Accept failed");
	if (clients.size() >= BACKLOG || fds_poll.size() >= BACKLOG + 1)
	{
		std::cout <<  "[ ERROR ] Too many client connections" << std::endl;
		close(fd_connection);
	}

	poll_connection = (struct pollfd){fd_connection, POLLIN, 0};
	fds_poll.push_back(poll_connection);
	iter = fds_poll.begin();

	Client	*client = new Client(fd_connection, connection_addr.sin_addr, this);
	client->is_online = false;
	client->ping_request = true;
	clients.push_back(client);
	
	std::cout << CYAN << "[Server]: Client " << fd_connection << " from " << inet_ntoa(connection_addr.sin_addr)
		<< ":" << ntohs(connection_addr.sin_port) << " connected." << RESET << std::endl;
}

void	Server::remove_client(std::vector<pollfd>::iterator &iter)
{
	Client *client = *(get_client_byfd(iter->fd));
	client->is_online = false;

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
	char buffer[BUFFER + 1];
	bzero(&buffer, sizeof(buffer));
	int read_bytes;

	std::vector<Client*>::iterator client = get_client_byfd(iter->fd);
	if ((*client)->mode & VALID_CLIENT)
	{
		(*client)->time_now = std::time(NULL);
		(*client)->ping_request = false;
	}

	read_bytes = recv(iter->fd, buffer, BUFFER, 0);
	if (read_bytes <= 0)
	{
		remove_client(iter);
		return ;
	}
	std::string recv_message(buffer);
	while (recv_message[recv_message.length() - 1] == '\n')
		recv_message.erase(recv_message.end() - 1);
	std::cout << "[Server] " << recv_message << RESET << std::endl;
	msg.clear();

	buffer[read_bytes] = '\0';
	(*client)->receive_leftovers += buffer;
	size_t pos = (*client)->receive_leftovers.find('\n');
	while (pos != std::string::npos)
	{
		msg = (*client)->receive_leftovers.substr(0, pos);
		(*client)->receive_leftovers = (*client)->receive_leftovers.substr(pos + 1);

		std::vector<std::string> split = split_msg();
		for (std::vector<std::string>::iterator it = split.begin(); it != split.end(); it++)
		{
			Message message(*it);
			std::vector<Server::ptr>::iterator cmd = get_command(message.cmd); 
			if (cmd != commands.end())
			{
				if (message.cmd == "PASS" || message.cmd == "QUIT" || message.cmd == "USER" || message.cmd == "NICK" 
					|| (*client)->mode & VALID_CLIENT)
					(this->*(*cmd))((*client)->fd, message);
				else
					send_message((*client)->fd, get_time() + (*client)->nickname + " :Not connected to server\r\n");
			}
			else if (!message.cmd.empty())
				send_message((*client)->fd, ERR_UNKNOWNCOMMAND((*client)->get_realname(), message.cmd));
		}
		pos = (*client)->receive_leftovers.find('\n');
	}
	
}

std::vector<std::string> Server::split_msg(void)
{
	std::stringstream stream(this->msg);
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
	fcntl(server_fd, F_SETFL, O_NONBLOCK);

	time_t		time_now;
	time(&time_now);
	time_init = ctime(&time_now);
	time_init.resize(time_init.size() - 1);

	std::cout << GREEN << "IRC Server started and listening at " << server_port << RESET << std::endl;

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
					(*(get_client_byfd(iter->fd)))->send_leftovers.clear();
			}
			else if (iter->revents & POLLHUP)
				remove_client(iter);
		}
		// check_ping();
	}
	std::cout << CYAN << "Ending server. Bye!" << RESET << std::endl;
}

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
	if (name == "LIST") return std::find(commands.begin(), commands.end(),  &Server::list);
	if (name == "NAMES") return std::find(commands.begin(), commands.end(),  &Server::names);
	if (name == "OPER") return std::find(commands.begin(), commands.end(),  &Server::oper);
	if (name == "KILL") return std::find(commands.begin(), commands.end(),  &Server::kill);
	if (name == "SHUTDOWN") return std::find(commands.begin(), commands.end(),  &Server::shutdown);
	return (commands.end());
}

void Server::save_commands()
{
	commands.push_back(&Server::pass);
	commands.push_back(&Server::nick);
	commands.push_back(&Server::user);
	commands.push_back(&Server::quit);
	commands.push_back(&Server::whois);
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
	commands.push_back(&Server::list);
	commands.push_back(&Server::names);
	commands.push_back(&Server::oper);
	commands.push_back(&Server::kill);
	commands.push_back(&Server::shutdown);
}

std::string clean_string(std::string line)
{
	std::string clean_line;

	while(isspace(line[0]))
		line.erase(0, 1);
	clean_line = line.substr(0, line.find_last_not_of(" \r\t\n") + 1);
	return  clean_line;
}

void Server::save_opers()
{
	std::string filename = "./conf/opers.conf";
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cout << "[Server] Error opening config file." << std::endl;
		return ;
	}
	std::string line;
	std::getline(file, line);
	if (line != "oper_nick, oper_pass")
	{
		std::cout << "[Server] Error server format in config file" << std::endl;
		return ;
	}
	while (std::getline(file, line))
	{
		if (line.find(',') == std::string::npos)
			continue ;
		std::string nick;
		std::string pass;
		nick = clean_string(line.substr(0, line.find(',')));
		pass = clean_string(line.substr(line.find(',') + 1));
		possible_opers.push_back((std::make_pair(nick, pass)));
	}
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
	// std::cout << GREY << get_time() << ": Sent to client [fd=" << fd << "] message:\n\t" << message << RESET << std::endl;
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
	channel->set_creation_time(unix_time());
	send_message(client->fd, RPL_JOIN(client->get_realname(), name));
	channel->broadcast_message(RPL_MODE(channel->get_name(), client->nickname, "+o " + client->nickname));
}

void	Server::join_channel(Client *client, std::string channel_name, std::string password) //TODO
{
	std::vector<Channel*>::iterator channel_it;
	Channel *channel;

	channel_it = get_channel_by_name(channel_name);
	if (channel_it == channels.end())
		return ;
	channel = *channel_it;

	if (client->is_in_channel(channel_name))
		return ;
	if (can_join_channel(client, channel, password) == false)
		return ;
	std::cout << RPL_JOIN(client->get_realname(), channel_name) << std::endl;
	client->join_channel(channel);
	channel->add_client(client);
	channel->broadcast_message(RPL_JOIN(client->get_realname(), channel_name));
	if (channel->get_topic().empty())
		send_message(client->fd, RPL_TOPIC(client->get_realname(), channel->get_name(), channel->get_topic()));
	send_message(client->fd, RPL_NAMREPLY(client->get_realname(), channel->get_name(), channel->get_list_of_clients(client)));
	send_message(client->fd, RPL_ENDOFNAMES(client->get_realname(), channel->get_name()));
}

bool	Server::can_join_channel(Client *client, Channel *channel, std::string password)
{
	if (channel->get_mode().k && password != channel->get_password()) {
		send_message(client->fd, ERR_BADCHANNELKEY(client->get_realname(), channel->get_name()));
		return false;
	}

	if (channel->get_mode().i && !client->is_invited_to(channel->get_name())) {
		send_message(client->fd, ERR_INVITEONLYCHAN(client->get_realname(), channel->get_name()));
		return false;
	}

	if (channel->get_mode().i) {
		client->remove_invitation(channel->get_name());
	}

	if (channel->get_mode().l && channel->get_max_clients() <= channel->get_current_clients()) {
		send_message(client->fd, ERR_CHANNELISFULL(client->get_realname(), channel->get_name()));
		return false;
	}

	return true;
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
	{
		if (flags_op[i] == 's' || flags_op[i] == 'n')
			continue;
		if (flags_op[i] == '+' || flags_op[i] == '-')
			continue;
		if (flags_op[i] != 'i' && flags_op[i] != 't' && flags_op[i] != 'k' && flags_op[i] != 'o' && flags_op[i] != 'l')
		{
			send_message(client->fd, ERR_UNKNOWNMODE(client->get_realname(), flags_op[i]));
			return false;
		}
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


int Server::get_operators_server(void)
{
	int num = 0;

	for (std::vector<Client*>::iterator iter = clients.begin(); iter != clients.end(); iter++)
	{
		if ((*iter)->is_oper == true)
			num++;
	}
	return num;
}


int Server::get_unknown_server(void)
{
	int num = 0;
	for (std::vector<Client*>::iterator iter = clients.begin(); iter != clients.end(); iter++)
	{
		if ((*iter)->mode & VALID_CLIENT)
			num++;
	}
	return (fds_poll.size() - num - 1);
}