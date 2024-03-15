#ifndef BOT_HPP
# define BOT_HPP

#include "irc_bot.hpp"

class Bot
{
	private:
		std::string					_nick;
		std::string					_channel_password;
		std::string					_port;
		std::string					_user;
		bool						online;
		std::string					serverAddress;
		int							_socket;
		struct pollfd				fd_poll;
		struct addrinfo				*servinfo;
		std::vector<std::string>	channels;
	public:
		// Orthodox canonical class
		Bot(void);
		~Bot(void);
		Bot(const Bot &cpy);
		Bot &operator=(const Bot &rhs);

		// Constructors
		Bot(std::string nick, std::string password, std::string port);

		// Methods
		void		initializeSocket(void);
		void		connectToServer(void);
		void		run(void);

		//Actions
		void		joinHandler(std::string msg);
		void		kickHandler(std::string msg);
		void		privmsgHandler(std::string msg);

		// Communication
		void		sendMessage(std::string msg);
		std::string	recvMessage(void);
		int			checkPoll(void);

		void		incomingMessage(void);

		void		removeMessage(void);
		// Send Info
		void		sendUserInfo(void);
		bool		sendPass(void);
		void		sendNick(void);
		void		sendUser(void);

		// Commands
		// * !help
		void	helpHandler(std::string nick, std::string channel);
		// * !nick <new_nick>
		void	nickHandler(std::string new_nick);
		// * !part <channel>
		void	partHandler(std::string channel);
		// * !time
		void	timeHandler(std::string channel);
		// // * !weather <city>
		// void	weatherHandler(std::string nick, std::string channel, std::string city);

		void	sendPrivmsg(std::string channel, std::string msg);


		// Channels
		void		addChannel(std::string channel);
		void		removeChannel(std::string channel);
		bool		isInChannel(std::string channel);

		// Getters
		std::string	getNick(void) const;
		std::string	getUser(void) const;
		std::string	getChannelPassword(void);
		std::string	getPort(void);
		int			getSocket(void);
		bool		onlineStatus(void);
		std::string	getServerAddress(void);

		// Setters
		void		setNick(std::string nick);
		void		setUser(std::string user);
		void		setChannelPassword(std::string password);
		void		setPort(std::string port);
		void		setSocket(int sock);
		void		setOnline(bool status);
		void		setServerAddress(std::string addr);
};

#endif
