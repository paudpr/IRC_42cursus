
#include "User.hpp"
// #include "Channel.hpp"
// #include "utils.hpp"
// #include "ModeFlag.hpp"

User::User() : is_online(false) {
	memset(this, 0, sizeof(*this));
}

User::User(int i_fd, in_addr address, Server* i_server) : hostname(inet_ntoa(address)), server(i_server), fd(i_fd), mode(0), is_online(false) 
{
	timestamp = std::time(NULL);
	ping_request = true;
}

User::User(const User &other) : is_online(false) { *this = other; }

User::~User() {}

User	&User::operator=(const User &other) {
	if (this == &other)
		return (*this);
	mode = other.mode;
	fd = other.fd;
	nickname = other.nickname;
	passwd = other.passwd;
	username = other.username;
	hostname = other.hostname;
	fullname = other.fullname;
	// channels = other.channels;
	return (*this);
}

User& User::get_user_by_fd(int fd, std::map<int, User> fds_users)
{
	for (std::map<int, User>::iterator iter = fds_users.begin(); iter != fds_users.end(); iter++)
	{
		if (iter->first == fd)
			return (iter->second);
	}
	throw std::runtime_error("[ ERROR ] Can't get User");	//si tiro excepcion rompo todo? cambiar por mensaje de error normal
}

std::string User::get_next_command(void)
{
	size_t pos = buffer.find('\n');
	if (pos == std::string::npos)
		return std::string();
	std::string command = buffer.substr(0, '\n');
	std::string aux = buffer.substr(pos + 1);
	buffer = aux;
	return command;
}

std::string& User::get_fullname()
{
	std::string fullname = nickname + "!" + username + "@" + hostname;
	return fullname;
}
