# pragma  once

#include  "ft_irc.hpp"

class Message
{
	friend class Client;
	friend class Server;

	public:
		~Message();
		Message(std::string& msg);
		Message(char* msg_char);
		Message& operator=(Message& other);

	private:
		std::string cmd;
		std::vector<std::string> args;
		std::string message;
		std::string prefix;
		std::string trailing;

		Message();
		Message(Message& copy);

};