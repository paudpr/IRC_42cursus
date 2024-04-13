#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "ft_irc.hpp"

//Clases
class Client;

// MODE - Change the channel’s mode:
// · i: Set/remove Invite-only channel -
// · t: Set/remove the restrictions of the TOPIC command to channel operators
// · k: Set/remove the channel key (password) -
// · o: Give/take channel operator privilege 
// · l: Set/remove the user limit to channel -
typedef struct s_channel_modes
{
	bool	i;
	bool	t;
	bool	k;
	bool	o;
	bool	l;
}	t_channel_modes;

class Channel
{
	private:
		std::string				topic;
		std::string				name;
		t_channel_modes			mode;
		int						op_fd;
		std::vector<int>		operators_fds;
		std::string				creation_time;

		//Modes
		int						max_clients;
		int						current_clients;
		std::string				password;
		void					init_modes(void);
		std::vector<Client*>	clients;

		//Mode

		bool					change_mode_i(bool mode);
		bool					change_mode_t(bool mode);
		bool					change_mode_k(bool mode, std::string password);
		bool					change_mode_o(bool mode, std::string name_op, Client *client);
		bool					change_mode_l(bool mode, int limit);

	public:
		Channel();
		~Channel();
		Channel(const Channel& copy);
		Channel& operator=(const Channel& other);
		Channel(std::string name, Client *client);

		// Setter
		void			set_topic(const std::string topic);
		void			set_name(const std::string name);
		void			set_mode(t_channel_modes mode);
		void			set_op(int op_fd);
		void			set_max_clients(int users);
		void			set_current_clients(int users);
		void			set_password(std::string password);
		void			set_creation_time(std::string creation_time);

		//Getters
		std::string		get_topic(void) const;
		std::string		get_name(void) const;
		t_channel_modes	get_mode(void) const;
		int				get_op(void) const;
		int				get_max_clients(void) const;
		int				get_current_clients(void) const;
		std::string		get_password(void) const;
		std::string		get_creation_time(void) const;
		std::vector<int>	get_operators_fds()  const;

		//Channel
		Client*			find_client_by_nick(std::string nick);
		void			add_client(Client* client);
		std::string		check_modes(std::vector<std::string> args, Client *client);
		bool			handle_mode(bool status, char mode, std::string arg, Client *client);

		//JOIN
		std::string		get_list_of_clients(Client *client);

		//Send message
		void			broadcast_message(std::string message);
		void			send_message(Client *client, std::string message);

		//PART
		void			remove_client(Client *client);

		//Operators
		void			add_operator(Client *client);
		void			remove_operator(Client *client);
		bool			is_operator(Client *client);

		//Users
		void			increase_clients(void);
		void			decrease_clients(void);

		//Names
		std::string		get_names_list(Client *client);

		//Mode
		std::string		get_mode_string(void);
};
#endif