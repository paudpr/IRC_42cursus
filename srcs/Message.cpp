
#include "Message.hpp"

Message::Message()
{}

Message::~Message()
{}

void get_prefix(std::string& msg, std::string &prefix)
{
	size_t i;
	for (i = 0; i < msg.size(); i++)
	{
		if (msg[i] == ' ')
			break  ;
	}
	prefix = msg.substr(0, i);
	msg = msg.substr(i + 1, msg.size() - prefix.size());
}

void get_command(std::string& msg, std::string& command)
{
	// std::cout  <<  "no está empty-" << msg << "-" <<  msg.size() <<  "-"<<  std::endl;
	if (*(msg.begin()) != *(msg.end()) && *(msg.begin()) == '/')
		msg.erase(0, 1);
	while (!msg.empty() && (msg.begin() != msg.end() && isspace(*msg.begin())))
		msg.erase(0, 1);
	if (msg.empty())
		return ;
	size_t i;
	for (i = 0; i < msg.size(); i++)
	{
		if (msg[i] == ' ')
			break  ;
	}
	command = msg.substr(0, i);
	msg.erase(0, command.size() + 1);
}

void get_args(std::string& msg, std::vector<std::string>& args, std::string& trailing)
{
	while (!msg.empty() && (msg.begin() != msg.end() && isspace(*msg.begin())))
		msg.erase(0, 1);
	std::istringstream stream(msg);
	std::string arg;
	while (stream >> arg)
		args.push_back(arg);
	trailing = args[args.size() - 1];
}

Message::Message(std::string& msg)
{
	message = msg;
	// if (msg.find('\n') != std::string::npos)
	//  	std::cout << "no debería pasar asi que lo que sea " << std::endl;
	while (msg.begin() != msg.end() && isspace(*(msg.begin())))
		msg.erase(0, 1);
	// if (*(msg.begin()) == ':')
	// 	get_prefix(msg, prefix);
	// std::cout << "-" << msg << "-"  << (int)(msg[0])<< "-" << msg.size() << "-" << std::endl; 
	if(!msg.empty())
		get_command(msg, cmd);

	// std::cout << "salgo de get commando " << std::endl;
	if(!msg.empty())
		get_args(msg, args, trailing);
}

Message& Message::operator=(Message& other)
{
	if (this == &other)
		return *this;
	cmd = other.cmd;
	args = other.args;
	message = other.message;
	prefix = other.prefix;
	trailing = other.trailing;
	return *this;
} 

bool Message::operator==(const Message& other) const { return (cmd == other.cmd); }

bool Message::operator<(const Message& other) const {  return (cmd < other.cmd); }