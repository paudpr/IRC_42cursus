
#include "Client.hpp"

Client::Client() : is_online(false) {
	memset(this, 0, sizeof(*this));
}

Client::Client(int input_fd, in_addr address, Server* i_server) : hostname(inet_ntoa(address)), server(i_server), fd(input_fd), mode(0), is_online(false) 
{
	ping_request = true;
}

Client::Client(const Client &other) : is_online(false) { *this = other; }

Client::~Client() {}

Client	&Client::operator=(const Client &other) {
	if (this == &other)
		return (*this);
	mode = other.mode;
	fd = other.fd;
	nickname = other.nickname;
	password = other.password;
	username = other.username;
	hostname = other.hostname;
	realname = other.realname;
	return (*this);
}

Client& Client::get_client_by_fd(int fd, std::map<int, Client> fds_clients)
{
	for (std::map<int, Client>::iterator iter = fds_clients.begin(); iter != fds_clients.end(); iter++)
	{
		if (iter->first == fd)
			return (iter->second);
	}
	std::cout << "[ ERROR ] Can't get Client" << std::endl;
	return (fds_clients.end()->second);
}

std::string Client::get_next_command(void)
{
	size_t pos = buffer.find('\n');
	if (pos == std::string::npos)
		return std::string();
	std::string command = buffer.substr(0, '\n');
	std::string aux = buffer.substr(pos + 1);
	buffer = aux;
	return command;
}

std::string Client::get_realname()
{
	std::string realname = nickname + "!" + username + "@" + hostname;
	return realname;
}
