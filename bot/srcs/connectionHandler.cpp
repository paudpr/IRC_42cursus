#include "irc_bot.hpp"

void	Bot::initializeSocket(void)
{
	struct addrinfo hints;

	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(getServerAddress().c_str(), getPort().c_str(), &hints, &servinfo) != 0)
		throw std::runtime_error("[ ERROR ] Getaddrinfo failed");
	_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (_socket < 0)
		throw std::runtime_error("[ ERROR ] Socket failed");
	fd_poll = (struct pollfd){_socket, POLLIN, 0};
	int	aux = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &aux, sizeof(int)) < 0)
	{
		close(_socket);
		throw std::runtime_error("[ ERROR ] Setsockopt failed");
	}
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, &aux, sizeof(int)) < 0)
	{
		close(_socket);
		throw std::runtime_error("[ ERROR ] Setsockopt failed");
	}
}

void	Bot::connectToServer(void)
{
	while (connect(_socket, servinfo->ai_addr, servinfo->ai_addrlen) != 0)
	{
		std::cout << "Trying to connect to " << getServerAddress() << " on port " << getPort() << std::endl;
		std::cout << "Retrying in 5 seconds" << std::endl;
		sleep(5);
	}
}

int	Bot::checkPoll(void)
{
	return (poll(&fd_poll, 1, POLL_TIMEOUT_MS));
}