#include "irc_bot.hpp"

void	showLeaks(void)
{
	system("leaks -q ircbot");
}

void	showFds(void)
{
	system("lsof -c ircbot");
}

int	main(int ac, char **av)
{
	std::string nick;
	std::string password;
	std::string port;

	#ifdef LEAKS
	atexit(showLeaks);
	#endif
	#ifdef FD
	atexit(showFds);
	#endif
	if (!check_arguments(ac, av))
		return (1);

	port = av[1];
	password = av[2];
	if (ac == 4)
		nick = av[3];
	else
		nick = "irc_bot";
	
	Bot bot(nick, password, port);
	try {
		bot.initializeSocket();
		bot.connectToServer();
		bot.run();
	} catch (std::exception &e) {
		std::cout << BIRed << e.what() << " - " << strerror(errno) << Color_Off << std::endl;
	}
	bot.freeBot();
	return (0);
}