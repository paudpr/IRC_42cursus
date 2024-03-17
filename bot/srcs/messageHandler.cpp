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
	if (bytes <= 0)
		throw std::runtime_error("[ ERROR ] Recv failed");
	msg = std::string(buffer);
	return (msg);
}

void	Bot::incomingMessage(void)
{
	std::string msg = recvMessage();
	if (msg.find("\r\n") != std::string::npos)
		msg.erase(msg.find("\r\n"), 2);
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
	else if (command.find("weather") != std::string::npos)
		weatherHandler(channel, arg);
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

// * !weather <city>
void	Bot::weatherHandler(std::string channel, std::string msg)
{
	std::vector<std::string> result;
	std::vector<std::string>::iterator it;
	std::string city = joinSplit(split(msg, " "));
	int	sockfd = connectToWeatherAPI();
	if (sockfd < 0)
		return ;
	std::string request = "GET /data/2.5/weather?q=" + city + "&appid=" + weatherAPI + " HTTP/1.1\r\nHost: api.openweathermap.org\r\n\r\n";
	if (sendHTTPRequest(sockfd, request) == false)
		return ;
	std::string response = receiveHTTPRequest(sockfd);
	if (response.empty())
		return ;
	close(sockfd);
	std::string json = std::string(response);
	json = json.substr(json.find("{"));
	std::string weather = "Weather in " + city + " ";
	sendPrivmsg(channel, weather);
	json = json.substr(json.find("{"));
	std::cout << json << std::endl;
	result = parseWeather(json);
	for (it = result.begin(); it != result.end(); ++it)
		sendPrivmsg(channel, *it);
}

std::vector<std::string>	Bot::parseWeather(std::string json)
{
	std::vector<std::string> result;
	std::string msg = "";
	std::vector<std::string> allJSON = splitJson(json);
	std::string main = getByKey(allJSON, "main");
	std::string weather = getByKey(allJSON, "weather");
	std::vector<std::string> weatherJSON = splitJson(weather);
	std::vector<std::string> mainJSON = splitJson(main);
	std::string temp = getByKey(mainJSON, "temp");
	std::string temp_max = getByKey(mainJSON, "temp_max");
	std::string temp_min = getByKey(mainJSON, "temp_min");
	std::string description = getByKey(weatherJSON, "description");
	temp = std::to_string(int(round((std::stof(temp) - 273.15) * 100) / 100));
	temp_max = std::to_string(int(round((std::stof(temp_max) - 273.15) * 100) / 100));
	temp_min = std::to_string(int(round((std::stof(temp_min) - 273.15) * 100) / 100));
	// sendPrivmsg(channel, "Temperature - " + temp + "°C");
	// sendPrivmsg(channel, "Max - " + temp_max + "°C");
	// sendPrivmsg(channel, "Min - " + temp_min + "°C");
	// sendPrivmsg(channel, "Description - " + description);
	result.push_back("Temperature - " + temp + "°C");
	result.push_back("Max - " + temp_max + "°C");
	result.push_back("Min - " + temp_min + "°C");
	result.push_back("Description - " + description);
	return (result);
}

void	Bot::sendPrivmsg(std::string channel, std::string msg)
{
	sendMessage("PRIVMSG " + channel + " :" + msg);
}

// Weather 
bool	Bot::sendHTTPRequest(int sockfd, std::string request)
{
	if (send(sockfd, request.c_str(), request.size(), 0) < 0)
	{
		perror("send");
		close(sockfd);
		return (false);
	}
	return (true);
}

std::string	Bot::receiveHTTPRequest(int sockfd)
{
	char	buffer[1024];
	std::string response;
	int		bytes;
	bzero(buffer, 1024);
	if ((bytes = recv(sockfd, buffer, 1024, 0)) < 0)
	{
		perror("recv");
		close(sockfd);
		return ("");
	}
	response = std::string(buffer, bytes); // asigna el contenido del buffer a response
	return (response);
}