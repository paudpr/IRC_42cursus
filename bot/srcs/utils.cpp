#include "irc_bot.hpp"

bool	check_port(std::string port)
{
	int	port_int;
	for (size_t i = 0; i < port.size(); i++)
	{
		if (!std::isdigit(port[i]))
			return (false);
	}
	port_int = std::stoi(port);
	if (port_int < 0 || port_int > 65535)
		return (false);
	return (true);
}

bool	check_password(std::string password)
{
	if (password.size() < 5)
		return (false);
	return (true);
}

bool	check_arguments(int ac, char **av)
{
	if (ac != 3 && ac != 4)
	{
		std::cerr << "Usage: " << av[0] << " <port> <password> [nickname]" << std::endl;
		return (false);
	}
	if (!check_port(av[1]))
	{
		std::cerr << "Invalid port" << std::endl;
		return (false);
	}
	if (!check_password(av[2]))
	{
		std::cerr << "Invalid password" << std::endl;
		return (false);
	}
	return (true);
}

std::vector<std::string> split(std::string str, std::string delimiter)
{
	std::vector<std::string> result;
	size_t pos = 0;
	std::string token;
	while ((pos = str.find(delimiter)) != std::string::npos)
	{
		token = str.substr(0, pos);
		result.push_back(token);
		str.erase(0, pos + delimiter.length());
	}
	result.push_back(str);
	return (result);
}

std::string joinSplit(std::vector<std::string> split)
{
	std::string result;
	std::vector<std::string>::iterator it;
	for (it = split.begin(); it != split.end(); ++it)
	{
		result += *it;
		if (it + 1 != split.end())
			result += " ";
	}
	return (result);
}

std::string getTime(void)
{
	time_t		now = time(0);
	struct tm	tstruct;
	char		buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	return (buf);
}