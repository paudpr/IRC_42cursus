
#pragma once

# include "ft_irc.hpp"
# include "Client.hpp"
# include "Message.hpp"

class Client;
class Message;


class Server {
	friend class Client;
	friend class Message;

	public:
		typedef void (Server::*ptr)(const int&, Message&);

		Server();
		~Server();
		Server(const Server& copy);
		Server& operator=(const Server& other);

		bool check_args(char *argv_port, char *argv_pass);
		void init();
		void start();
		std::string get_hostname();
		void add_client(std::vector<pollfd>::iterator &iter);
		void remove_client(std::vector<pollfd>::iterator &iter);
		void do_communications(std::vector<pollfd>::iterator &iter);
		std::vector<Client*>::iterator get_client_byfd(int fd);
		std::vector<std::string> split_msg(void);
		void check_ping();
		std::string print_time();

		void pass(const int& fd, Message& message);
		void nick(const int& fd, Message& message);
		void user(const int& fd, Message& message);
		void whois(const int& fd, Message& message);

		bool check_availability(std::string& nick, std::string& client_nick);
		bool check_valid_user(Client *client);

		void send_message(const int& fd, std::string message);

	private:
		std::map<std::string, std::string>	oper_status;
		std::vector<ptr> commands;
		std::vector<Client*> clients;
		std::vector<pollfd> fds_poll;

		socklen_t		server_size;
		socklen_t		conn_size;
		sockaddr_in		server_addr;
		sockaddr_in		conn_addr;
		std::string		hostname;
		std::string		server_passwd;
		std::string		timestamp_creation;
		int				server_fd;
		int				server_port;
		bool			online;
		std::string		msg;

		void save_commands();
		std::vector<ptr>::iterator get_command(std::string& name);
		

};