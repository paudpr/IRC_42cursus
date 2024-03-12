
#pragma once

# include "ft_irc.hpp"
# include "Client.hpp"
# include "Message.hpp"
# include "Channel.hpp"

class Client;
class Message;
class Channel;


class Server {
	friend class Client;
	friend class Message;
	friend class Channel;

	public:
		typedef void (Server::*ptr)(const int&, Message&);

		Server();
		~Server();
		Server(const Server& copy);
		Server& operator=(const Server& other);

		//conf server
		bool check_args(char *argv_port, char *argv_pass);
		void init();
		void start();

		//commands
		void pass(const int& fd, Message& message);
		void nick(const int& fd, Message& message);
		void user(const int& fd, Message& message);
		void whois(const int& fd, Message& message);
		void quit(const int& fd, Message& message);
		void ping(const int& fd, Message& message);
		void pong(const int& fd, Message& message);
		void lusers(const int& fd, Message& message);
		void motd(const int& fd, Message& message);
		void join(const int& fd, Message& message);
		void mode(const int& fd, Message& message);
		void privmsg(const int& fd, Message& message);
		void part(const int& fd, Message& message);
		void topic(const int& fd, Message& message);
		void invite(const int& fd, Message& message);
		void kick(const int& fd, Message& message);
		void list(const int& fd, Message& message);

		//utils commands
		bool check_availability(std::string& nick, std::string& client_nick);
		bool check_valid_user(Client *client, Message& message);

		//messaging
		void send_message(const int& fd, std::string message);
		//INVITE
		Client*			get_client_by_nickname(std::string nickname);
	private:
		std::vector<Channel*>	channels;
		std::vector<ptr> commands;
		std::vector<Client*> clients;
		std::vector<pollfd> fds_poll;

		int				server_fd;
		int				server_port;
		bool			online;
		socklen_t		server_size;
		socklen_t		connection_size;
		sockaddr_in		server_addr;
		sockaddr_in		connection_addr;
		std::string		hostname;
		std::string		server_passwd;
		std::string		time_init;
		std::string		msg;

		std::string get_hostname();

		//working server
		void add_client(std::vector<pollfd>::iterator &iter);
		void remove_client(std::vector<pollfd>::iterator &iter);
		void do_communications(std::vector<pollfd>::iterator &iter);

		//utils server
		void save_commands();
		void check_ping();
		std::vector<Client*>::iterator get_client_byfd(int fd);
		std::vector<std::string> split_msg(void);
		std::vector<ptr>::iterator get_command(std::string& name);
		


		void send_welcome(Client*  client, Message& message);

		bool find_channel(std::string name);
		std::vector<Channel*>::iterator	get_channel_by_name(std::string name);

		//channel
		void							create_channel(std::string name, Client *client);
		void							join_channel(Client *client, std::string channel_name, std::string password);
		bool							can_join_channel(Client *client, Channel *channel, std::string password);
		bool							is_valid_channel_name(std::string name);

		void							add_channel(Channel *channel);

		bool							client_exists(std::string nick);
		
		//MODE
		bool							is_valid_mode(std::string mode, Client *client);
		void							change_mode(std::string mode, Client *client, Channel *channel);

		//Channel
		void							remove_channel(Channel *channel);
};