# include "irc_bot.hpp"

// Send info
void	Bot::sendUserInfo(void)
{
	if (!sendPass())
	{
		std::cerr << BIRed << "Invalid password" << Color_Off << std::endl;
		return ;
	}
	sendNick();
	sendUser();
}

bool	Bot::sendPass(void)
{
	std::string msg;
	sendMessage("PASS " + getChannelPassword());
	if (checkPoll() < 0)
		throw std::runtime_error("[ ERROR ] Poll");
	else if (fd_poll.revents & POLLIN)
	{
		msg = recvMessage();
		if (msg.find("464") != std::string::npos)
			return (false);
	}
	return (true);
}

void	Bot::sendNick(void)
{
	std::string msg;
	std::string nick = getNick();
	while (true)
	{
		sendMessage("NICK " + nick);
		if (checkPoll() < 0)
			throw std::runtime_error("[ ERROR ] Poll");
		if (fd_poll.revents & POLLIN)
		{
			msg = recvMessage();
			if (msg.find("433") == std::string::npos)
				break;
			nick += "_";
		}
		else
			break;
	}
	if (nick != getNick())
		setNick(nick);
	setOnline(true);
}

void	Bot::sendUser(void)
{
	sendMessage("USER " + getUser() + " 0 * :realname");
}

// Communication
void	Bot::sendMessage(std::string msg)
{
	msg += "\r\n";
	int	fd = getSocket();
	int total = 0;
	int	bytesleft = msg.size();
	int	byteswritten;
	while (total < (int)msg.size())
	{
		byteswritten = send(fd, msg.c_str() + total, bytesleft, 0);
		if (byteswritten == -1)
			break;
		total += byteswritten;
		bytesleft -= byteswritten;
	}
}

std::string	Bot::recvMessage(void)
{
	char	buffer[1024];
	int		bytes;
	std::string	msg;

	bzero(buffer, 1024);
	bytes = recv(getSocket(), buffer, 1024, 0);
	if (bytes < 0)
		throw std::runtime_error("[ ERROR ] Recv failed");
	msg = std::string(buffer);
	return (msg);
}

void	Bot::incomingMessage(void)
{
	std::string msg = recvMessage();
	if (msg.find(RPL_INVITING) != std::string::npos)
		joinHandler(msg);
	else if (msg.find("KICK") != std::string::npos)
		kickHandler(msg);
	else if (msg.find("PRIVMSG") != std::string::npos)
		privmsgHandler(msg);
}

void	Bot::removeMessage(void)
{
	while (fd_poll.revents & POLLIN)
	{
		if (checkPoll() < 0)
			throw std::runtime_error("[ ERROR ] Poll");
		recvMessage();
	}
}


// * !help
// * !nick <new_nick>
// * !part <channel>
// * !time
// * !weather <city>
//TODO: add more commands
void	Bot::privmsgHandler(std::string msg)
{
	std::string nick;
	std::string channel;
	std::string command;
	std::string arg;
	std::vector<std::string> tokens = split(msg, " ");
	if (tokens[3].find("!") == std::string::npos)
		return ;
	nick = tokens[0].erase(0, 1);
	channel = tokens[2];
	command = tokens[3].erase(0, 1);
	if (tokens.size() > 4)
		arg = tokens[4];
	else
		arg = "";
	if (command.find("help") != std::string::npos)
		helpHandler(nick, channel);
	else if (command.find("nick") != std::string::npos)
		nickHandler(arg);
	else if (command.find("part") != std::string::npos)
		partHandler(channel);
	else if (command.find("time") != std::string::npos)
		timeHandler(channel);
	// else if (command == "weather")
		// weatherHandler(nick, channel, arg);
}

// Commands
// * !help
//TODO: add more commands
void	Bot::helpHandler(std::string nick, std::string channel)
{
	sendPrivmsg(channel, BOLD + nick + " Commands: ");
	sendPrivmsg(channel, BOLD + std::string("!help") + RESET + " - Display this message");
	sendPrivmsg(channel, BOLD + std::string("!nick <new_nick>") + RESET + " - Change nickname");
	sendPrivmsg(channel, BOLD + std::string("!part") + RESET + " - Leave channel");
	sendPrivmsg(channel, BOLD + std::string("!time") + RESET + " - Display current time");
	sendPrivmsg(channel, BOLD + std::string("!weather <city>") + RESET + " - Display weather in city");
}

// * !nick <new_nick>
void	Bot::nickHandler(std::string new_nick)
{
	if (new_nick.empty())
		return ;
	while (true)
	{
		sendMessage("NICK " + new_nick);
		if (checkPoll() < 0)
			throw std::runtime_error("[ ERROR ] Poll");
		if (fd_poll.revents & POLLIN)
		{
			std::string msg = recvMessage();
			if (msg.find("433") == std::string::npos)
				break;
			new_nick += "_";
		}
		else
			break;
	}
}

// * !part <channel>
void	Bot::partHandler(std::string channel)
{
	removeChannel(channel);
	sendMessage("PART " + channel);
}

// * !time
void	Bot::timeHandler(std::string channel)
{
	std::string time = "Current time: " + getTime();
	sendPrivmsg(channel, time);
}

// // * !weather <city>
// void	Bot::weatherHandler(std::string msg)
// {

// }

void	Bot::sendPrivmsg(std::string channel, std::string msg)
{
	sendMessage("PRIVMSG " + channel + " :" + msg);
	std::cout << BIRed << "PRIVMSG " << channel << " :" << msg << Color_Off << std::endl;
}