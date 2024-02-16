
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
		// typedef map<int, Client>						std::map<int, Client>;
		// typedef map<stringKey, Client *>				Clientnick_map;
		// typedef map<const stringKey, class Channel>	channel_map;
		// typedef vector<pollfd>						std::vector<pollfd>;
		// typedef std::map<std::string, std::string>					opCredentials_t;

		// typedef map<char, ModeFlag *>				flag_map;
		// typedef map<int, string>					std::map<int, std::string>;

		Server();
		~Server();
		Server(const Server& copy);
		Server&			operator=(const Server& other);

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

	private:
		std::map<std::string, std::string>	oper_status;
		std::vector<ptr> commands;
		std::vector<Client*> clients;


		
		std::vector<pollfd> fds_poll;

		socklen_t		server_size;
		socklen_t		conn_size;
		sockaddr_in		server_addr;
		sockaddr_in		conn_addr;
		std::string			hostname;
		std::string			server_passwd;
		std::string			timestamp_creation;
		// char			buffer[BUFFER];
		int				server_fd;
		int				server_port;
		bool			online;
		std::string		msg;

		void save_commands();
		std::vector<ptr>::iterator get_command(std::string& name);
		void send_message(const int& fd, std::string message);
		
		
		std::map<int, std::string> recv_leftovers;	//sin uso, rompen la conexión por algún motivo
		std::map<int, std::string> send_leftovers;
};

// extern bool *killSwitch;