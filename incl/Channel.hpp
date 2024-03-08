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

		//Modes
		int						max_clients;
		int						current_clients;
		std::string				password;
		void					set_modes_false(void);
		std::vector<Client*>	clients;	

		//Mode

		void					change_mode_i(bool mode);
		void					change_mode_t(bool mode);
		void					change_mode_k(bool mode, std::string password);
		void					change_mode_o(bool mode, Client *client);
		void					change_mode_l(bool mode, int limit);

		//Channel
		Client*					find_client_by_nick(std::string nick);
	public:
		Channel();
		~Channel();
		Channel(const Channel& copy);
		Channel& operator=(const Channel& other);
		Channel(std::string name, int op_fd);

		// Setter
		void			set_topic(const std::string topic);
		void			set_name(const std::string name);
		void			set_mode(t_channel_modes mode);
		void			set_op(int op_fd);
		void			set_max_clients(int users);
		void			set_current_clients(int users);
		void			set_password(std::string password);

		//Getters
		std::string		get_topic(void) const;
		std::string		get_name(void) const;
		t_channel_modes	get_mode(void) const;
		int				get_op(void) const;
		int				get_max_clients(void) const;
		int				get_current_clients(void) const;
		std::string		get_password(void) const;

		//channel
		void			add_client(Client* client);
		std::string		handle_mode(std::vector<std::string> args);

		//JOIN
		std::string		get_list_of_clients(void);

		//Send message
		void			broadcast_message(std::string message);
};
#endif