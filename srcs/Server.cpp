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
		// channels = other.channels;
		// commands = other.commands;
		server_size = other.server_size;
		conn_size = other.conn_size;
		server_addr = other.server_addr;
		conn_addr = other.conn_addr;
		hostname = other.hostname;
		server_fd = other.server_fd;
		server_port = other.server_port;
		oper_status = other.oper_status;
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
	conn_size = sizeof(conn_addr);
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

	fd_connection = accept(fds_poll.begin()->fd, (sockaddr*)&conn_addr, &conn_size);
	if (fd_connection < 0)
		throw std::runtime_error("[ ERROR ] Accept failed");

	poll_connection = (struct pollfd){fd_connection, POLLIN, 0};

	if (clients.size() >= BACKLOG || fds_poll.size() >= BACKLOG + 1)
	{
		// send_msg( "Too many clients connected already"); // enviar mensaje al cliente de que no se puede conectar
		std::cout <<  "[ ERROR ] Too many client connections" << std::endl;
		close(fd_connection);
		// throw std::runtime_error("[ ERROR ] Too many client connections");
	}

	fcntl(fd_connection, F_SETFL, O_NONBLOCK);
	fds_poll.push_back(poll_connection);
	iter = fds_poll.begin();

	Client	*client = new Client(fd_connection, conn_addr.sin_addr, this);
	client->is_online = true;
	clients.push_back(client);

	std::cout << CYAN << "Client: " << fd_connection << " from " << inet_ntoa(conn_addr.sin_addr)
		<< ":" << ntohs(conn_addr.sin_port) << " connected." << RESET << std::endl;

	send_message(iter->fd, "Now connected to esteproyectoponlocomosea");
}

void	Server::remove_client(std::vector<pollfd>::iterator &iter)
{
	std::vector<Client*>::iterator client = get_client_byfd(iter->fd);
	(*client)->is_online = false;
	
	//abandonar channels

	std::cout << RED << "[Server]: Client " << iter->fd
		<< " from " << inet_ntoa(conn_addr.sin_addr)
		<< ":" << ntohs(conn_addr.sin_port)
		<< " disconnected" << RESET << std::endl;

	close(iter->fd);
	fds_poll.erase(iter);
	delete (*client);
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

	read_bytes = recv(iter->fd, buffer, BUFFER, 0);
	msg.clear();
	if (read_bytes < 0)
		throw std::runtime_error("[ ERROR ] Failed recv/read"); //tengo que salir o solo mandar error, desconectar y continuar???
	if (read_bytes == 0)
	{
		remove_client(iter);
		return ;
	}

	std::vector<Client*>::iterator client = get_client_byfd(iter->fd);	//version sentido comun
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
			if (message.cmd == "PASS" || message.cmd == "USER" || message.cmd == "NICK" 
				|| (*client)->mode & VALID_CLIENT)
			{	
				std::cout << message.message << std::endl;
				(this->*(*cmd))((*client)->fd, message);
			}
	}
}

std::vector<Server::ptr>::iterator Server::get_command(std::string& name)
{
	if (name == "PASS") return std::find(commands.begin(), commands.end(), &Server::pass);
	if (name == "NICK") return std::find(commands.begin(), commands.end(), &Server::nick);
	if (name == "USER") return std::find(commands.begin(), commands.end(), &Server::user);
	if (name == "WHOIS") return std::find(commands.begin(), commands.end(), &Server::whois);
	return (commands.end());
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

	std::cout << GREEN << "IRC Server started and listening at " << server_port << RESET << std::endl;

	// time_t		current_time;
	// time(&current_time);
	// timestamp_creation = ctime(&current_time);
	// timestamp_creation.resize(timestamp_creation.size() - 1);
	// std::cout << GREEN << timestamp_creation << RESET << std::endl;

	while (online) {
		int poll_count = poll(fds_poll.begin().base(), (nfds_t)fds_poll.size(), POLL_TIMEOUT_MS);
		if (poll_count < 0)
			throw std::runtime_error("[ ERROR ] Poll");
		for (std::vector<pollfd>::iterator iter = fds_poll.begin(); iter < fds_poll.end(); iter++)
		{
			// if (iter->revents != POLLIN && iter->revents != 17)
			// {
			// 	std::cout << ORANGE << "[ ERROR ] Revents" << RESET << std::endl;
			// 	online = false;
			// 	break ;
			// }
			if (iter->revents & POLLHUP)
				remove_client(iter);
			else if (iter->revents & POLLIN)
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
		}
		check_ping();
	}
	std::cout << BLUE << "Ending server. Bye!" << std::endl;
}

void Server::check_ping()
{
	std::cout <<  BLUE << "ping " <<  RESET << std::endl;
}

void Server::save_commands()
{
	commands.push_back(&Server::nick);
	commands.push_back(&Server::pass);
	commands.push_back(&Server::user);
}

std::string Server::print_time()
{
	std::string str = "print time ->";
	return  str;
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
	std::cout << LIGHT_CYAN << print_time() << "Sent to client [fd=" << fd << "] message:" << std::endl;
	std::cout << "\t" << message << RESET << std::endl;
}
