#include "irc_bot.hpp"

void	Bot::freeBot(void)
{
	free(servinfo->ai_addr);
	free(servinfo);
}

// Getters
std::string	Bot::getNick(void) const
{
	return _nick;
}

std::string	Bot::getUser(void) const
{
	return _user;
}

std::string	Bot::getChannelPassword(void)
{
	return _channel_password;
}

std::string	Bot::getPort(void)
{
	return _port;
}

int	Bot::getSocket(void)
{
	return (this->_socket);
}

bool	Bot::onlineStatus(void)
{
	return (this->online);
}

std::string	Bot::getServerAddress(void)
{
	return (this->serverAddress);
}

// Setters
void	Bot::setNick(std::string nick)
{
	_nick = nick;
}

void	Bot::setUser(std::string user)
{
	_user = user;
}

void	Bot::setChannelPassword(std::string password)
{
	_channel_password = password;
}

void	Bot::setPort(std::string port)
{
	_port = port;
}

void	Bot::setSocket(int sock)
{
	this->_socket = sock;
}

void	Bot::setOnline(bool status)
{
	this->online = status;
}

void	Bot::setServerAddress(std::string addr)
{
	this->serverAddress = addr;
}