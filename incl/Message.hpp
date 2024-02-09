# pragma  once

#include  "ft_irc.hpp"

class Message
{
	friend class User;
	friend class Server;

	public:
		~Message();
		Message(std::string& msg);
		Message(char* msg_char);
		Message& operator=(Message& other);
		bool operator==(const Message& other) const;
		bool operator<(const Message& other) const;

	private:
		std::string cmd;
		std::vector<std::string> args;
		std::string message;
		std::string prefix;
		std::string trailing;

		Message();
		Message(Message& copy);

};