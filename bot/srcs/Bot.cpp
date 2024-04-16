#include "Bot.hpp"

std::string Bot::weatherAPI = "";

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

std::string	getToken(void)
{
	std::string	filename = "./conf/API.conf";
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("[BOT] Error: can't get token");
	std::string line;
	std::getline(file, line);
	while (line.find_last_of('\n') != std::string::npos)
		line.erase(line.length(), -1);
	if (line.empty())
		throw std::runtime_error("[BOT] Error: No token");
	return (line);
}

Bot::Bot(std::string nick, std::string password, std::string port)
	: _nick(nick), _channel_password(password), _port(port)
{
	setUser("irc_bot");
	setOnline(false);
	setSocket(-1);
	setServerAddress("127.0.0.1");
	weatherAPI = getToken();
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
		else if (fd_poll.revents & POLLOUT)
			std::cout << "Sending message" << std::endl;
		else if (fd_poll.revents & POLLERR)
			std::runtime_error("[ ERROR ] Poll error");
		else if (fd_poll.revents & POLLHUP)
			std::runtime_error("[ ERROR ] Poll HUP");
		else if (fd_poll.revents & POLLNVAL)
			std::runtime_error("[ ERROR ] Poll NVAL");
	}
}

void	Bot::joinHandler(std::string msg)
{
	std::string channel;
	channel = msg.substr(msg.find("#"));
	std::cout << "Is in channel: " << ((isInChannel(channel))? "Si" : "No") << std::endl;
	if (isInChannel(channel))
		return ;
	addChannel(channel);
	sendMessage("JOIN " + channel);
}

void	Bot::kickHandler(std::string msg)
{
	std::string channel;
	std::vector<std::string> splitMsg = split(msg, " ");
	std::vector<std::string>::iterator it;

	for (it = splitMsg.begin(); it != splitMsg.end(); it++)
	{
		if ((*it).find("#") != std::string::npos)
		{
			channel = *it;
			break ;
		}
	}
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
	std::vector <std::string>::iterator it;
	for (it = channels.begin(); it != channels.end(); it++)
		if (*it == channel)
			return true;
	return false;
}