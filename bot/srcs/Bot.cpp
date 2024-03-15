#include "Bot.hpp"

Bot::Bot(void)
	: _nick(""), _channel_password(""), _port("")
{
	setUser("user");
}

Bot::~Bot(void)
{
	if (getSocket() > 0)
		close (_socket);
}

Bot::Bot(const Bot &cpy)
{
	*this = cpy;
}

Bot &Bot::operator=(const Bot &rhs)
{
	if (this != &rhs)
	{
		_nick = rhs._nick;
		_user = rhs._user;
		_channel_password = rhs._channel_password;
		_port = rhs._port;
	}
	return *this;
}


Bot::Bot(std::string nick, std::string password, std::string port)
	: _nick(nick), _channel_password(password), _port(port)
{
	setUser("irc_bot");
	setOnline(false);
	setSocket(-1);
	setServerAddress("127.0.0.1");

}



void	Bot::run(void)
{
	removeMessage();
	sendUserInfo();
	while (onlineStatus())
	{
		if (checkPoll() < 0)
			throw std::runtime_error("[ ERROR ] Poll");
		if (fd_poll.revents & POLLIN)
			incomingMessage();
		// else if (fd_poll.revents & POLLOUT)
		// // Send message
		// {
		// 	std::cout << "Sending message" << std::endl;
		// }
		// else if (fd_poll.revents & POLLERR)
		// // Error
		// {
		// 	std::cout << "Error" << std::endl;
		// }
		// else if (fd_poll.revents & POLLHUP)
		// // Hangup
		// {
		// 	std::cout << "Hangup" << std::endl;
		// }
		// else if (fd_poll.revents & POLLNVAL)
		// // Invalid request
		// {
		// 	std::cout << "Invalid request" << std::endl;
		// }
	}
}

void	Bot::joinHandler(std::string msg)
{
	std::string channel;
	channel = msg.substr(msg.find("#"));
	if (isInChannel(channel))
		return ;
	addChannel(channel);
	sendMessage("JOIN " + channel);
}

void	Bot::kickHandler(std::string msg)
{
	std::string channel;
	channel = msg.substr(msg.find("#"));
	if (!isInChannel(channel))
		return ;
	removeChannel(channel);
}


// Channels
void	Bot::addChannel(std::string channel)
{
	channels.push_back(channel);
}

void	Bot::removeChannel(std::string channel)
{
	std::vector<std::string>::iterator it = std::find(channels.begin(), channels.end(), channel);
	if (it != channels.end())
		channels.erase(it);
}

bool	Bot::isInChannel(std::string channel)
{
	std::vector<std::string>::iterator it = std::find(channels.begin(), channels.end(), channel);
	if (it != channels.end())
		return (true);
	return (false);
}