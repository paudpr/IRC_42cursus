#include "ft_irc.hpp"
#include "Server.hpp"

bool Server::check_args(char *argv_port, char *argv_pass)
{
	std::string port;
	std::string pass;

	port = argv_port;
	pass = argv_pass;
	for (std::string::iterator iter = port.begin(); iter != port.end(); ++iter)
	{
		if (!isdigit(*iter))
		{
			std::cout << "[ ERROR ] Port not valid. Must be only digits" << std::endl;
			return false;
		}
	}
	int port_value = std::stoi(port);
	if (port_value <= 1024 || port_value > 65535)			//All ports below 1024 are RESERVED (unless you’re the superuser) <- lo gestiono????
	{
		std::cerr << "[ ERROR ] Port not valid. Must be a number between 0 and 65535" << std::endl;
		return false;
	}
	if (pass.size() < 5)
	{
		std::cout << "[ ERROR ] Password length not long enough. Must be at least 5 characters" << std::endl;
		return false;
	}
	for (std::string::iterator iter = pass.begin(); iter != pass.end(); ++iter)
	{
		if (!isprint(*iter))
		{
			std::cout << "[ ERROR ] Password must consist of printable characters" << std::endl;
			return false; 
		}
	}
	server_port = port_value;
	server_passwd = argv_pass;
	return true;
}

int		main(int argc, char** argv) {

	if (argc != 3)
	{
		std::cout << "[ ERROR ] Invalid arguments. Must follow this structure: \n" 
		<< "\t./ircserv <port> <password>" << std::endl;
		return 1;
	}
	Server server;
	if (!server.check_args(argv[1], argv[2]))
		return 1;
	try {
		server.init();
		server.start();
	} catch (std::exception &e) {
		std::cout << e.what() << "-" << strerror(errno) << std::endl;
	}
	return 0;
}
