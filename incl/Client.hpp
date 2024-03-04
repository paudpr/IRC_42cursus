#pragma once

# include "ft_irc.hpp"

class Server;

class Client {
	friend class Server;
	friend class Channel;

	public:
		Client();
		Client(int input_fd, in_addr address, Server* i_server);
		Client(const Client &other);
		~Client();

		Client &operator=(const Client &other);
		bool operator==(const  Client &other);

		std::string get_next_command(void);
		std::string get_realname();
		Client& get_client_by_fd(int fd, std::map<int, Client> fds_clients);

	private:
		std::string				nickname;
		std::string				password;
		std::string				username;
		std::string				hostname;
		std::string				realname;
		Server*				server;
		time_t				time_init;
		time_t				time_now;
		int					fd;
		char				mode;
		bool				is_online;
		bool				ping_request;
		std::string			ping_token;

		int pass_tries; 	//quitar si versión de Server::pass sin comprobación de tries

		std::string send_leftovers;
		std::string receive_leftovers;
		std::string	msg;
		std::string buffer;

		std::vector<Channel *>	channels;
};
