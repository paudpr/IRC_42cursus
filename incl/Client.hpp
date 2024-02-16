#pragma once

# include "ft_irc.hpp"

class Server;

class Client {
	friend class Server;
	friend class Channel;

	public:
		// typedef map<string, Channel*>		channel_map;
		// typedef Server::flag_map			flag_map;

		Client();
		Client(int i_fd, in_addr address, Server* i_server);
		Client(const Client &other);
		~Client();

		Client &operator=(const Client &other);
		// void				setMode(const Message &cmd);
		// void				displayMode() const;

		// const string		&getNickname() const;
		std::string get_next_command(void);
		std::string get_fullname();
		Client& get_client_by_fd(int fd, std::map<int, Client> fds_clients);

	private:
		// channel_map			channels;
		// static flag_map		flags;
		std::string				nickname;
		std::string				passwd;
		std::string				name;
		std::string				hostname;
		std::string				fullname;
		std::string				ping_key;
		Server*				server;
		time_t				timestamp;
		time_t				timestamp_login;
		int					fd;
		char				mode;
		bool				is_online;
		bool				ping_request;

		std::string send_leftovers;
		std::string receive_leftovers;
		std::string	msg;
		std::string buffer;
};
