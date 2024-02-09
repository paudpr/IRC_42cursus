#pragma once

# include "ft_irc.hpp"

class Server;

class User {
	friend class Server;
	friend class Channel;

	public:
		// typedef map<string, Channel*>		channel_map;
		// typedef Server::flag_map			flag_map;

		User();
		User(int i_fd, in_addr address, Server* i_server);
		User(const User &other);
		~User();

		User &operator=(const User &other);
		// void				setMode(const Message &cmd);
		// void				displayMode() const;

		// const string		&getNickname() const;
		std::string get_next_command(void);
		std::string& get_fullname();
		User& get_user_by_fd(int fd, std::map<int, User> fds_users);

	private:
		// channel_map			channels;
		// static flag_map		flags;
		std::string				nickname;
		std::string				passwd;
		std::string				username;
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
