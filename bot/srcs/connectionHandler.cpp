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
	if (connect(_socket, servinfo->ai_addr, servinfo->ai_addrlen) != 0)
	{
		throw std::runtime_error("[ ERROR ] Connect failed");
	}
}

int	Bot::checkPoll(void)
{
	return (poll(&fd_poll, 1, POLL_TIMEOUT_MS));
}

int	Bot::connectToWeatherAPI(void)
{
	std::string hostname = "api.openweathermap.org";
	int	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		return (-1);
	}
	struct hostent *server = gethostbyname(hostname.c_str());
	if (server == NULL)
	{
		perror("gethostbyname");
		return (-1);
	}
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	server_addr.sin_port = htons(80);

	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("connect");
		return (-1);
	}
	return (sockfd);
}
